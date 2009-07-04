<?php
// $Id: dummy.inc,v 1.1 2009/04/10 10:00:15 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Dummy backend

/**
 * Implements a dummy backend for use as a template for developing storage
 * adapters for the Bitcache module.
 */
class Bitcache_DummyRepository extends Bitcache_Repository implements Iterator {
  public $table;

  function __construct(array $options = array()) {
    $this->options = $options;
    $this->create();
  }

  function create() {}

  function rename($old_name, $new_name, array $options = array()) {}

  function destroy() {}

  function open($mode = 'r') {}

  function close() {}

  // Iterator interface

  function rewind() {
    $this->open();
    // TODO
  }

  function key() {
    // TODO
  }

  function current() {
    // TODO
  }

  function next() {
    // TODO
  }

  function valid() {
    // TODO
  }

  // Bitcache_Repository interface

  function count() {
    return 0; // TODO
  }

  function size($physical = TRUE) {
    return 0; // TODO
  }

  function exists($id) {
    if ($this->open('r')) {
      // TODO
    }
    return FALSE;
  }

  function resolve($id, array $options = array()) {
    return NULL; // TODO
  }

  function get($id) {
    if ($this->open('r')) {
      // TODO
    }
    return FALSE;
  }

  function put($id, $data) {
    $id = !$id ? bitcache_id($data) : $id;
    if ($this->open('w')) {
      // TODO
    }
    return FALSE;
  }

  function delete($id) {
    if ($this->open('w')) {
      // TODO
    }
    return FALSE;
  }
}
