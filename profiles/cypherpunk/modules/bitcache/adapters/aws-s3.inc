<?php
// $Id: aws-s3.inc,v 1.2 2009/05/19 15:27:22 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Amazon S3 backend

/**
 * Implements an Amazon S3 backend.
 *
 * @see https://s3.amazonaws.com/
 * @see http://tarzan-aws.com/
 */
class Bitcache_S3Repository extends Bitcache_Repository implements IteratorAggregate {
  public $bucket, $acl;

  static function load() {
    require_once './' . drupal_get_path('module', 'bitcache') . '/vendor/tarzan/tarzan.class.php';
    require_once './' . drupal_get_path('module', 'bitcache') . '/vendor/tarzan/s3.class.php';
  }

  function __construct(array $options = array()) {
    self::load();
    $this->s3      = new AmazonS3($options['access_key'], $options['secret_key']);
    $this->bucket  = $options['bucket'];
    $this->acl     = isset($options['acl']) ? $options['acl'] : S3_ACL_PUBLIC;
    $this->options = $options;
    $this->create();
  }

  function create() {
    $this->s3->enable_ssl = isset($options['ssl']) ? (bool)$options['ssl'] : TRUE;

    if (!$this->s3->if_bucket_exists($this->bucket)) {
      $this->s3->create_bucket($this->bucket);
      $this->s3->set_bucket_acl($this->bucket, $this->acl);
    }
  }

  function rename($old_name, $new_name, array $options = array()) {
    //$this->s3->rename_bucket($this->bucket, $new_name);
  }

  function destroy() {
    //$this->s3->delete_bucket($this->bucket); // NOTE: a bucket has to be empty to be deleted
  }

  function open($mode = 'r') {}

  function close() {}

  // Iterator interface

  public function getIterator() {
    $streams = array();
    if (($response = $this->s3->list_objects($this->bucket)) && $response->isOK()) {
      foreach ($response->body->Contents as $object) {
        if (preg_match(BITCACHE_ID_FORMAT, $id = (string)$object->Key)) {
          $streams[$id] = new Bitcache_Stream($id, NULL, array('size' => intval((string)$object->Size), 'type' => NULL)); // FIXME
        }
      }
    }
    return new ArrayIterator($streams);
  }

  // Bitcache_Repository interface

  function count() {
    return (int)$this->s3->get_bucket_size($this->bucket);
  }

  function size($physical = TRUE) {
    return (int)$this->s3->get_bucket_filesize($this->bucket);
  }

  function exists($id) {
    return $this->s3->if_object_exists($this->bucket, $id);
  }

  function resolve($id, array $options = array()) {
    return $this->s3->get_object_url($this->bucket, $id); // TODO: BitTorrent support
  }

  function get($id) {
    if (($response = $this->s3->get_object($this->bucket, $id))) {
      if ($response->isOK()) {
        return new Bitcache_Stream($id, $response->body, array('size' => (int)$response->header['content-length'], 'type' => $response->header['content-type'])); // FIXME
      }
    }
    return FALSE;
  }

  function put($id, $data, array $options = array()) {
    $id = !$id ? bitcache_id($data) : $id;
    if (!$this->exists($id) && ($response = $this->s3->create_object($this->bucket, array('filename' => $id, 'body' => $data, 'contentType' => isset($options['type']) ? $options['type'] : 'application/octet-stream', 'acl' => $this->acl)))) {
      if ($response->isOK()) {
        $this->created($id, $data);
        return $id;
      }
    }
    return FALSE;
  }

  function delete($id) {
    if (($response = $this->s3->delete_object($this->bucket, $id))) {
      if ($response->isOK()) {
        $this->deleted($id);
        return TRUE;
      }
    }
    return FALSE;
  }
}
