<?php
// $Id: bitcache.rules.inc,v 1.1 2009/07/01 08:55:37 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (data types)

function bitcache_rules_data_type_info() {
  return array(
    'bitcache_stream' => array(
      'module'          => 'Bitcache',
      'label'           => t('Bitcache stream'),
      'class'           => 'bitcache_rules_data_type_stream',
      'identifiable'    => TRUE,
      'savable'         => FALSE,
      'uses_input_form' => FALSE,
    ),
  );
}

class bitcache_rules_data_type_stream extends rules_data_type {
  function load($id) {
    return bitcache_get($id);
  }

  function get_identifier() {
    $stream = &$this->get();
    return $stream->id();
  }
}

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (events)

/**
 * Implementation of hook_rules_event_info().
 */
function bitcache_rules_event_info() {
  return array(
    'bitcache_insert' => array(
      'module'    => 'Bitcache',
      'label'     => t('Bitstream created'),
      'help'      => t('This event is triggered after a new bitstream has been uploaded.'),
      'arguments' => array(
        'stream'     => array('label' => t('Bitstream'), 'type' => 'bitcache_stream'),
      ),
    ),
    'bitcache_delete' => array(
      'module'    => 'Bitcache',
      'label'     => t('Bitstream deleted'),
      'help'      => t('This event is triggered after a bitstream has been deleted.'),
      'arguments' => array(
        'stream'     => array('label' => t('Bitstream'), 'type' => 'bitcache_stream'),
      ),
    ),
  );
}

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (conditions)

/**
 * Implementation of hook_rules_condition_info().
 */
function bitcache_rules_condition_info() {
  return array(
    // TODO: Bitstream exists in repository
    // TODO: Bitstream size is zero
    // TODO: Bitstream size is...
    // TODO: Bitstream MIME type is...
  );
}

//////////////////////////////////////////////////////////////////////////////
// Rules API hooks (actions)

/**
 * Implementation of hook_rules_action_info().
 */
function bitcache_rules_action_info() {
  return array(
    'bitcache_rules_action_delete' => array(
      'module'    => 'Bitcache',
      'label'     => t('Delete bitstream'),
      'help'      => t(''),
      'arguments' => array(
        'stream'     => array('label' => t('Bitstream to delete'), 'type' => 'bitcache_stream'),
      ),
    ),
    'bitcache_rules_action_delete_from' => array(
      'module'    => 'Bitcache',
      'label'     => t('Delete bitstream from repository'),
      'help'      => t(''),
      'arguments' => array(
        'stream'     => array('label' => t('Bitstream to delete'), 'type' => 'bitcache_stream'),
        'repository' => array('label' => t('Repository name'), 'type' => 'string'),
      ),
    ),
    'bitcache_rules_action_push_to' => array(
      'module'    => 'Bitcache',
      'label'     => t('Push bitstream to repository'),
      'help'      => t(''),
      'arguments' => array(
        'stream'     => array('label' => t('Bitstream to copy'), 'type' => 'bitcache_stream'),
        'repository' => array('label' => t('Repository name'), 'type' => 'string'),
      ),
    ),
    // TODO: Delete all bitstreams in repository
    // TODO: Synchronize two repositories
  );
}

/**
 * Action: Delete bitstream.
 */
function bitcache_rules_action_delete($stream) {
  bitcache_delete($stream->id());
}

/**
 * Action: Delete bitstream from repository.
 */
function bitcache_rules_action_delete_from($stream, $repository) {
  if (($repo = bitcache_get_repository($repository))) {
    $repo->delete($stream->id());
  }
}

/**
 * Action: Push bitstream to repository.
 */
function bitcache_rules_action_push_to($stream, $repository) {
  if (($repo = bitcache_get_repository($repository))) {
    $repo->put($stream->id(), $stream->data());
  }
}
