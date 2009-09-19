<?php
// $Id: openpgp_gpg.inc,v 1.3 2009/09/19 17:17:48 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// GnuPG API implementation

/**
 * GNU Privacy Guard (GnuPG) wrapper API for PHP 5.2+.
 *
 * @see http://www.gnupg.org/
 */
class GnuPG extends OpenPGP {
  static $version;
  static $output;

  /**
   * Returns TRUE if GnuPG is installed and the GnuPG settings have been correctly configured.
   */
  function is_available() {
    return is_dir(self::homedir()) && is_executable(self::binpath());
  }

  /**
   * Returns the GnuPG version number.
   */
  function version() {
    if (empty(self::$version) && ($output = self::exec('version')) && !empty($output)) {
      if (preg_match('!^gpg \(GnuPG\) (.*)$!', $output[0], $matches)) {
        self::$version = $matches[1];
      }
    }
    return self::$version;
  }

  /**
   * Encrypts the given plaintext using the specified public key ID(s).
   */
  function encrypt($plaintext, $key_id, array $options = array()) {
    // Our trust database is not going to be of much use, so tell GnuPG to
    // always trust. The way to do this changed around version 1.2.3.
    $options = array_merge($options, version_compare(self::version(), '1.2.3', '<') ? array('always-trust' => TRUE) : array('trust-model'  => 'always'));

    // Encrypt for one or multiple recipients:
    if ($key_id) {
      $options['recipient'] = $key_id;
    }

    return self::exec_with_io('encrypt', $plaintext, array_merge(array('armor' => TRUE), $options));
  }

  /**
   * Decrypts the given ciphertext using the specified key ID.
   */
  function decrypt($ciphertext, $key_id = NULL, array $options = array()) {
    return self::exec_with_io('decrypt', $ciphertext, array_merge(array('armor' => TRUE), $options));
  }

  /**
   * Returns an array of key IDs/titles of the keys in the public keyring.
   */
  function get_keys($op = 'titles', $key_id = '') {
    static $columns = array('type', 'validity', 'length', 'algorithm', 'id', 'created_at', 'expires_at', 'local_id', 'trust', 'user_id', 'signature_class', 'capabilities', 'unspecified'); // FIXME

    $keys = array();
    if (($output = self::exec('list-public-keys', array('with-colons' => TRUE), $key_id)) && is_array($output)) {
      foreach ($output as $key) {
        $key = explode(':', $key);
        if ($key[0] == 'pub') { // public keys only
          $key = (object)array_combine($columns, $key);
          $keys[$key->id] = ($op == 'titles' ? (substr($key->id, -8) . ' ' . $key->user_id . ' (' . $key->created_at . ')') : $key);
        }
      }
    }
    return $keys;
  }

  /**
   * TODO
   */
  function generate_key(array $options = array()) {
    // TODO
  }

  /**
   * TODO
   */
  function delete_key($key_id, array $options = array()) {
    // TODO
  }

  /**
   * TODO
   */
  function export_key($key_id, array $options = array()) {
    if (($output = self::exec('export', array('armor' => TRUE), $key_id)) && !empty($output)) {
      return implode("\n", $output);
    }
  }

  /**
   * TODO
   */
  function import_key($key_data, array $options = array()) {
    if (($key_ids = self::import_keys($key_data, $options)) && is_array($key_ids) && !empty($key_ids)) {
      list($key_id) = array_keys($key_ids);
      return $key_id;
    }
  }

  /**
   * TODO
   */
  function import_keys($key_data, array $options = array()) {
    $options['quiet'] = NULL; // prevent '--quiet' being output
    if (($process = self::exec_with_pipes('import', $pipes, $options)) && is_resource($process)) {
      list($stdin, $stdout, $stderr) = $pipes;

      foreach (!is_array($key_data) ? array($key_data) : $key_data as $key_text) {
        fwrite($stdin, $key_text);
        fwrite($stdin, "\n");
      }
      fclose($stdin);
      $out = stream_get_contents($stdout);
      fclose($stdout);
      $err = stream_get_contents($stderr);
      fclose($stderr);

      if (($result = proc_close($process)) === 0) {
        $key_ids = array();
        foreach (explode("\n", $err) as $line) {
          if (preg_match('/^\w+: key ([0-9A-Fa-f]+):[^"]+"([^"]*)"/', $line, $match)) {
            list(, $key_id, $user_id) = $match;

            // Attempt to expand the returned 64-bit key IDs to the full key fingerprints:
            if (($keys = self::get_keys(NULL, $key_id))) {
              foreach ($keys as $key_fp => $key) {
                if ((strpos($key_fp, $key_id) === strlen($key_fp) - strlen($key_id)) && $key->user_id == $user_id) {
                  $key_id = $key_fp;
                  break;
                }
              }
            }

            $key_ids[$key_id] = $user_id;
          }
        }
        return $key_ids;
      }

      return FALSE;
    }
  }

  /**
   * Invokes the 'gpg' executable with easy stdin/stdout handling.
   */
  function exec_with_io($command, $input, array $options = array()) {
    if (!self::is_available()) return FALSE;

    if (($process = self::exec_with_pipes($command, $pipes, $options)) && is_resource($process)) {
      list($stdin, $stdout, $stderr) = $pipes;

      fwrite($stdin, $input);
      fclose($stdin);
      $output = stream_get_contents($stdout);
      fclose($stdout);
      self::$output = stream_get_contents($stderr);
      fclose($stderr);

      $result = proc_close($process);
      return $result === 0 ? $output : FALSE;
    }
  }

  /**
   * Invokes the 'gpg' executable using proc_open().
   */
  function exec_with_pipes($command, array &$pipes = NULL, array $options = array()) {
    if (!self::is_available()) return FALSE;

    self::$output = NULL;
    $arguments = array_slice(func_get_args(), 3);

    return proc_open(self::command_line($command, $options, $arguments),
      array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'), 2 => array('pipe', 'w')), $pipes);
  }

  /**
   * Invokes the 'gpg' executable using exec().
   */
  function exec($command, array $options = array()) {
    if (!self::is_available()) return FALSE;

    self::$output = NULL;
    $arguments = array_slice(func_get_args(), 2);

    exec(self::command_line($command, $options, $arguments), $output, $result);
    self::$output = implode("\n", $output);
    return $result === 0 ? $output : FALSE;
  }

  /**
   * Assembles the full 'gpg' command line to be invoked.
   */
  function command_line($command, array $options = array(), array $arguments = array()) {
    $result = self::binpath();

    foreach (self::options($options) as $k => $vs) {
      foreach (is_array($vs) ? $vs : array($vs) as $v) {
        if (is_numeric($k)) {
          $result .= ' ' . escapeshellarg((string)$v);
        }
        else if (is_bool($v)) {
          $result .= ' --' . ($v ? '' : 'no-') . $k;
        }
        else if (!is_null($v)) {
          $result .= ' --' . $k . ' ' . escapeshellarg((string)$v);
        }
      }
    }

    if ($command) {
      $result .= ' --' . (string)$command;
    }

    // Append any explicit command-line arguments, if given:
    if (($arguments = array_filter(array_map('strval', $arguments), 'strlen'))) {
      $result = trim($result . ' ' . implode(' ', array_map('escapeshellarg', $arguments)));
    }

    return $result;
  }

  /**
   * Determines the default command-line options for 'gpg'.
   */
  function options(array $options = array()) {
    $defaults  = array('batch' => TRUE, 'verbose' => FALSE, 'tty' => FALSE, 'quiet' => TRUE);
    $defaults += array('permission-warning' => FALSE, 'homedir' => self::homedir());

    if (file_exists($defaults['homedir'])) {
      // If we know that the path already exists, canonicalize it using
      // realpath() to obtain the actual absolute path. In some environments
      // doing this prevents a problem described in the troubleshooting
      // section of INSTALL.txt.
      $defaults['homedir'] = realpath($defaults['homedir']);
    }

    if (!is_writable($defaults['homedir'])) {
      // Prevent the need to write .gnupg/random_seed, allowing the
      // .gnupg directory to be set read-only:
      $defaults['random-seed-file'] = FALSE;
    }

    if (($file = self::variable('options')) && !isset($options['options'])) {
      $defaults['options'] = $file;
    }

    if (($mode = self::variable('compatibility'))) {
      $defaults[$mode] = TRUE;
    }

    if (self::variable('no_emit_version') && !isset($options['emit-version'])) {
      $defaults['emit-version'] = FALSE;
    }

    if (self::variable('throw_keyids') && !isset($options['throw-keyids'])) {
      $defaults['throw-keyids'] = TRUE;
    }

    return array_merge($defaults, $options);
  }

  /**
   * Attempts to locate GnuPG's 'gpg' binary in $PATH or, failing that, in
   * various well-known system path locations.
   */
  function binpath($binary = 'gpg') {
    if (($path = self::variable('exec'))) {
      return $path;
    }

    if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') { // running on Windows?
      return $binary; // let's not even go there
    }

    if (($path = exec('type -P ' . escapeshellarg($binary), $output, $result)) && $result === 0) {
      return $path;
    }

    foreach (array('/opt/local/bin', '/usr/local/bin', '/usr/bin') as $location) {
      if (@is_executable($path = $location . '/' . $binary)) {
        return $path;
      }
    }

    return $binary; // no go
  }

  /**
   * Returns the file system path where GnuPG stores its configuration and
   * keyrings.
   */
  function homedir() {
    return function_exists('variable_get') ? self::variable('homedir', drupal_get_path('module', 'openpgp') . '/.gnupg') : '~/.gnupg';
  }

  /**
   * TODO
   */
  function variable($name, $default = NULL) {
    return function_exists('variable_get') ? variable_get('openpgp_gpg_' . $name, $default) : $default;
  }
}
