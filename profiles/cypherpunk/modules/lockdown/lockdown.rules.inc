<?php
// $Id: lockdown.rules.inc,v 1.2 2009/06/26 17:13:00 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (rules)

function lockdown_rules_defaults() {
  $rules = $rule_sets = array();
  foreach (lockdown_get_levels() as $level) {
    $level = (object)$level;
    $rule_sets['lockdown_level_' . $level->name] = array(
      'status'     => 'default',
      'label'      => t('Lockdown level: !title', array('!title' => $level->title)),
      'arguments'  => array(),
      'categories' => array(t('Lockdown levels')),
    );
  }
  return compact('rules', 'rule_sets');
}

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (events)

/**
 * Implementation of hook_rules_event_info().
 */
function lockdown_rules_event_info() {
  return array(
    'lockdown_init' => array(
      'module'    => 'Lockdown',
      'label'     => t('HTTP request initiated'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_exit' => array(
      'module'    => 'Lockdown',
      'label'     => t('HTTP request completed'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_level_activate' => array(
      'module'    => 'Lockdown',
      'label'     => t('Lockdown level activated'),
      'help'      => t(''),
      'arguments' => array(
        'level'   => array(
          'label' => t('Lockdown level'),
          'type'  => 'string',
          'description' => t('Enter the machine-readable name of the lockdown level you wish to activate. The available options are: %levels.', array('%levels' => implode(', ', lockdown_get_levels('name')))),
        ),
      ),
    ),
    'lockdown_level_deactivate' => array(
      'module'    => 'Lockdown',
      'label'     => t('Lockdown level deactivated'),
      'help'      => t(''),
      'arguments' => array(
        'level'   => array(
          'label' => t('Lockdown level'),
          'type'  => 'string',
          'description' => t('Enter the machine-readable name of the lockdown level you wish to activate. The available options are: %levels.', array('%levels' => implode(', ', lockdown_get_levels('name')))),
        ),
      ),
    ),
  );
}

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (conditions)

/**
 * Implementation of hook_rules_condition_info().
 */
function lockdown_rules_condition_info() {
  return array(
    'lockdown_rules_condition_lockdown_level_is' => array(
      'module'    => 'Lockdown',
      'label'     => t('Lockdown level is'),
      'help'      => t('Evaluates to TRUE if the current lockdown level is equal to the given argument.'),
      'arguments' => array(
        'level'   => array(
          'label' => t('Lockdown level'),
          'type'  => 'string',
          'description' => t('Enter the machine-readable name of the lockdown level you wish to activate. The available options are: %levels.', array('%levels' => implode(', ', lockdown_get_levels('name')))),
        ),
      ),
    ),
    'lockdown_rules_condition_referrer_contains' => array(
      'module'    => 'Lockdown',
      'label'     => t('Referrer contains text'),
      'help'      => t(''),
      'arguments' => array(
        'text'    => array(
          'label' => t('Text'),
          'type'  => 'string',
        ),
      ),
    ),
    'lockdown_rules_condition_referrer_is_empty' => array(
      'module'    => 'Lockdown',
      'label'     => t('Referrer is unspecified'),
      'help'      => t('Evaluates to TRUE if the HTTP referrer is empty.'),
      'arguments' => array(),
    ),
    'lockdown_rules_condition_request_uri_contains' => array(
      'module'    => 'Lockdown',
      'label'     => t('Request URI contains text'),
      'help'      => t(''),
      'arguments' => array(
        'text'    => array(
          'label' => t('Text'),
          'type'  => 'string',
        ),
      ),
    ),
    'lockdown_rules_condition_request_method_is_get' => array(
      'module'    => 'Lockdown',
      'label'     => t('Request method is GET'),
      'help'      => t('Evaluates to TRUE if the HTTP request method is GET.'),
      'arguments' => array(),
    ),
    'lockdown_rules_condition_request_method_is_post' => array(
      'module'    => 'Lockdown',
      'label'     => t('Request method is POST'),
      'help'      => t('Evaluates to TRUE if the HTTP request method is POST.'),
      'arguments' => array(),
    ),
    'lockdown_rules_condition_request_method_is_put' => array(
      'module'    => 'Lockdown',
      'label'     => t('Request method is PUT'),
      'help'      => t('Evaluates to TRUE if the HTTP request method is PUT.'),
      'arguments' => array(),
    ),
    'lockdown_rules_condition_request_method_is_delete' => array(
      'module'    => 'Lockdown',
      'label'     => t('Request method is DELETE'),
      'help'      => t('Evaluates to TRUE if the HTTP request method is DELETE.'),
      'arguments' => array(),
    ),
    'lockdown_rules_condition_accept_language_has' => array(
      'module'    => 'Lockdown',
      'label'     => t('Visitor accepts a given language'),
      'help'      => t(''),
      'arguments' => array(
        'lang'    => array(
          'label' => t('Language code'),
          'type'  => 'string',
        ),
      ),
    ),
    'lockdown_rules_condition_user_agent_is_crawler' => array(
      'module'    => 'Lockdown',
      'label'     => t('Visitor is a web crawler'),
      'help'      => t('Evaluates to TRUE if the visitor\'s HTTP user agent indicates a web crawler (e.g. Googlebot).'),
      'arguments' => array(),
    ),
    'lockdown_rules_condition_user_agent_contains' => array(
      'module'    => 'Lockdown',
      'label'     => t('Visitor\'s HTTP user agent contains text'),
      'help'      => t(''),
      'arguments' => array(
        'text'    => array(
          'label' => t('Text'),
          'type'  => 'string',
        ),
      ),
    ),
    'lockdown_rules_condition_remote_addr_contains' => array(
      'module'    => 'Lockdown',
      'label'     => t('Visitor\'s IP address contains text'),
      'help'      => t(''),
      'arguments' => array(
        'text'    => array(
          'label' => t('Text'),
          'type'  => 'string',
        ),
      ),
    ),
  );
}

/**
 * Condition: Lockdown level is...
 */
function lockdown_rules_condition_lockdown_level_is($level) {
  return lockdown_get_level() == $level;
}

/**
 * Condition: Referrer contains text.
 */
function lockdown_rules_condition_referrer_contains($text) {
  return strpos($_SERVER['HTTP_REFERER'], $text) !== FALSE;
}

/**
 * Condition: Referrer is unspecified.
 */
function lockdown_rules_condition_referrer_is_empty() {
  return trim($_SERVER['HTTP_REFERER']) == '';
}

/**
 * Condition: Request method is GET.
 */
function lockdown_rules_condition_request_method_is_get() {
  return $_SERVER['REQUEST_METHOD'] == 'GET';
}

/**
 * Condition: Request method is POST.
 */
function lockdown_rules_condition_request_method_is_post() {
  return $_SERVER['REQUEST_METHOD'] == 'POST';
}

/**
 * Condition: Request method is PUT.
 */
function lockdown_rules_condition_request_method_is_put() {
  return $_SERVER['REQUEST_METHOD'] == 'PUT';
}

/**
 * Condition: Request method is DELETE.
 */
function lockdown_rules_condition_request_method_is_delete() {
  return $_SERVER['REQUEST_METHOD'] == 'DELETE';
}

/**
 * Condition: Request URI contains text.
 */
function lockdown_rules_condition_request_uri_contains($text) {
  return strpos($_SERVER['REQUEST_URI'], $text) !== FALSE;
}

/**
 * Condition: Visitor accepts a given language.
 */
function lockdown_rules_condition_accept_language_has($lang) {
  return strpos($_SERVER['HTTP_ACCEPT_LANGUAGE'], $lang) !== FALSE;
}

/**
 * Condition: Visitor's HTTP user agent contains text.
 */
function lockdown_rules_condition_user_agent_contains($text) {
  return strpos($_SERVER['HTTP_USER_AGENT'], $text) !== FALSE;
}

/**
 * Condition: Visitor is a web crawler.
 */
function lockdown_rules_condition_user_agent_is_crawler() {
  return FALSE; // TODO
}

/**
 * Condition: Visitor's IP address contains text.
 */
function lockdown_rules_condition_remote_addr_contains($text) {
  return strpos($_SERVER['REMOTE_ADDR'], $text) !== FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (actions)

/**
 * Implementation of hook_rules_action_info().
 */
function lockdown_rules_action_info() {
  return array(
    'lockdown_rules_action_activate_level' => array(
      'module'    => 'Lockdown',
      'label'     => t('Activate a lockdown level'),
      'help'      => t(''),
      'arguments' => array(
        'level'   => array(
          'label' => t('Lockdown level'),
          'type'  => 'string',
          'description' => t('Enter the machine-readable name of the lockdown level you wish to activate. The available options are: %levels.', array('%levels' => implode(', ', lockdown_get_levels('name')))),
        ),
      ),
    ),
    'lockdown_rules_action_delay_http' => array(
      'module'    => 'Lockdown',
      'label'     => t('Delay the HTTP response'),
      'help'      => t(''),
      'arguments' => array(
        'delay'   => array(
          'label' => t('Delay in seconds'),
          'type'  => 'number',
          'default value' => 5,
          'description' => t('Enter how many seconds you wish to delay the HTTP response with. During this time the visitor\'s browser will display a blank loading page. Note that long delays may exceed PHP\'s timeout settings, in which case PHP will terminate the page request before the whole delay has elapsed.'),
        ),
      ),
    ),
    'lockdown_rules_action_send_http_status' => array(
      'module'    => 'Lockdown',
      'label'     => t('Send a HTTP response status'),
      'help'      => t(''),
      'arguments' => array(
        'status'  => array(
          'label' => t('HTTP response code'),
          'type'  => 'number',
          'default value' => 200,
          'description' => t('Enter an HTTP status code to terminate the HTTP response with. Use <code>200</code> for an all-okay response, <code>404</code> for a page-not-found response, and so on.'),
        ),
      ),
    ),
    'lockdown_rules_action_send_http_content_type' => array(
      'module'    => 'Lockdown',
      'label'     => t('Send a HTTP content type header'),
      'help'      => t(''),
      'arguments' => array(
        'type'    => array(
          'label' => t('MIME content type'),
          'type'  => 'string',
          'default value' => 'text/html',
          'description' => t('Enter a MIME content type to send to the client in a <code>Content-Type</code> HTTP header.'),
        ),
      ),
    ),
    'lockdown_rules_action_terminate_http' => array(
      'module'    => 'Lockdown',
      'label'     => t('Terminate the HTTP connection'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_rules_action_terminate_session' => array(
      'module'    => 'Lockdown',
      'label'     => t('Terminate the HTTP session'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_rules_action_exec_sh' => array(
      'module'    => 'Lockdown',
      'label'     => t('Execute shell command'),
      'help'      => t(''),
      'arguments' => array(
        'script'  => array(
          'label' => t('Shell command'),
          'type'  => 'string',
          'description' => t('Enter the shell command to execute when this action is triggered. All text entered here will be piped directly to the <tt>/bin/sh</tt> shell.'),
        ),
      ),
    ),
    'lockdown_rules_action_exec_sql' => array(
      'module'    => 'Lockdown',
      'label'     => t('Execute SQL query'),
      'help'      => t(''),
      'arguments' => array(
        'query'   => array(
          'label' => t('SQL query'),
          'type'  => 'string',
          'description' => t('Enter the SQL query or command to execute when this action is triggered. Wrap table names with the \'{\' and \'}\' characters in the usual Drupal manner (for example, enter <code>{users}</code> for the <code>users</code> table).'),
        ),
      ),
    ),
    'lockdown_rules_action_exec_php' => array(
      'module'    => 'Lockdown',
      'label'     => t('Execute PHP code'),
      'help'      => t(''),
      'arguments' => array(
        'code'    => array(
          'label' => t('PHP code'),
          'type'  => 'string',
          'description' => t('Enter the PHP code to evaluate when this action is triggered. Do <strong>not</strong> prepend <code>&lt;?php</code> but rather just enter PHP code directly.'),
        ),
      ),
    ),
    'lockdown_rules_action_erase_watchdog' => array(
      'module'    => 'Lockdown',
      'label'     => t('Erase system events'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_rules_action_erase_admin_email' => array(
      'module'    => 'Lockdown',
      'label'     => t('Erase administrator\'s e-mail address'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_rules_action_erase_user_email' => array(
      'module'    => 'Lockdown',
      'label'     => t('Erase users\' e-mail addresses'),
      'help'      => t(''),
      'arguments' => array(),
    ),
    'lockdown_rules_action_erase_database' => array(
      'module'    => 'Lockdown',
      'label'     => t('Erase the Drupal database (!)'),
      'help'      => t('<strong>WARNING:</strong> This action will permanently and irrevocably erase all database tables in the Drupal database. Be absolutely sure you know what you are doing!'),
      'arguments' => array(),
    ),
  );
}

/**
 * Action: Activate a lockdown level.
 */
function lockdown_rules_action_activate_level($level) {
  if (in_array($level, lockdown_get_levels('name'))) {
    lockdown_set_level($level);
  }
}

/**
 * Action: Delay the HTTP response.
 */
function lockdown_rules_action_delay_http($delay = 5) {
  set_time_limit(0);
  sleep($delay);
}

/**
 * Action: Send a HTTP response status.
 */
function lockdown_rules_action_send_http_status($status = 200) {
  // HTTP responses from Drupal's includes/common.inc:
  $responses = array(
    100 => 'Continue', 101 => 'Switching Protocols',
    200 => 'OK', 201 => 'Created', 202 => 'Accepted', 203 => 'Non-Authoritative Information', 204 => 'No Content', 205 => 'Reset Content', 206 => 'Partial Content',
    300 => 'Multiple Choices', 301 => 'Moved Permanently', 302 => 'Found', 303 => 'See Other', 304 => 'Not Modified', 305 => 'Use Proxy', 307 => 'Temporary Redirect',
    400 => 'Bad Request', 401 => 'Unauthorized', 402 => 'Payment Required', 403 => 'Forbidden', 404 => 'Not Found', 405 => 'Method Not Allowed', 406 => 'Not Acceptable', 407 => 'Proxy Authentication Required', 408 => 'Request Time-out', 409 => 'Conflict', 410 => 'Gone', 411 => 'Length Required', 412 => 'Precondition Failed', 413 => 'Request Entity Too Large', 414 => 'Request-URI Too Large', 415 => 'Unsupported Media Type', 416 => 'Requested range not satisfiable', 417 => 'Expectation Failed',
    500 => 'Internal Server Error', 501 => 'Not Implemented', 502 => 'Bad Gateway', 503 => 'Service Unavailable', 504 => 'Gateway Time-out', 505 => 'HTTP Version not supported',
  );
  header('HTTP/1.1 ' . (int)$status . ' ' . $responses[$status]);
}

/**
 * Action: Send a HTTP content type header.
 */
function lockdown_rules_action_send_http_content_type($type) {
  header('Content-Type: ' . check_plain($type));
}

/**
 * Action: Terminate the HTTP connection.
 */
function lockdown_rules_action_terminate_http() {
  exit;
}

/**
 * Action: Terminate the HTTP session.
 */
function lockdown_rules_action_terminate_session() {
  global $user;

  if (!empty($user->uid)) {
    watchdog('user', 'Session terminated for %name.', array('%name' => $user->name));
  }

  sess_destroy_uid($user->uid);
  session_destroy();
  $user = drupal_anonymous_user();
}

/**
 * Action: Execute shell command.
 */
function lockdown_rules_action_exec_sh($script) {
  $script = trim(str_replace("\r", '', $script));

  if (!empty($script)) {
    if (($proc = popen('/bin/sh', 'w'))) { // FIXME?
      fwrite($proc, $script);
      fwrite($proc, "\n");
      $result = pclose($proc);
      // TODO: log to watchdog if $result != 0
    }
  }
}

/**
 * Action: Execute SQL query.
 */
function lockdown_rules_action_exec_sql($query) {
  $query = trim(str_replace("\r", '', $query));

  if (!empty($query)) {
    db_query($query);
  }
}

/**
 * Action: Execute PHP code.
 */
function lockdown_rules_action_exec_php($code) {
  $code = trim(str_replace("\r", '', $code));

  if (!empty($code)) {
    eval($code);
  }
}

/**
 * Action: Erase system events.
 */
function lockdown_rules_action_erase_watchdog() {
  if (module_exists('dblog')) {
    db_query("DELETE FROM {watchdog}"); // erase system messages
  }
}

/**
 * Action: Erase administrator's e-mail address.
 */
function lockdown_rules_action_erase_admin_email() {
  db_query("UPDATE {users} SET mail = NULL WHERE uid = 1");
}

/**
 * Action: Erase users' e-mail addresses.
 */
function lockdown_rules_action_erase_user_email() {
  db_query("UPDATE {users} SET mail = NULL WHERE uid > 1");
}

/**
 * Action: Erase the Drupal database.
 */
function lockdown_rules_action_erase_database() {
  if (TRUE) { // TODO: add a checkbox confirmation to rule?
    $updates = array();
    foreach (lockdown_get_tables() as $table) {
      db_drop_table($updates, $table);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// Rules API helpers

function lockdown_get_rules_for_event($event) {
  return lockdown_get_rules_for_set('event_' . $event);
}

function lockdown_get_rules_for_set($set) {
  $rules = lockdown_get_rules();
  return isset($rules[$set]) ? $rules[$set] : array();
}

function lockdown_get_rules() {
  $rules = array();
  foreach (rules_get_configured_items('rules') as $rule_id => $rule) {
    $rules[$rule['#set']][$rule_id] = $rule;
  }
  return $rules;
}
