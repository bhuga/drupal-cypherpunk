<?php
// $Id: bitcache.services.inc,v 1.1 2009/07/01 04:09:48 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Services API hooks

/**
 * Implementation of hook_service().
 */
function bitcache_service() {
  return array(
    array(
      '#method'   => 'bitcache.exists',
      '#callback' => 'bitcache_exists',
      '#return'   => 'boolean',
      '#args'     => array(
        array('#name' => 'id', '#type' => 'string', '#optional' => FALSE, '#description' => t('The bitstream\'s identifier (SHA-1 fingerprint).')),
      ),
      '#help'     => t('Determines whether a given bitstream exists.'),
    ),
    array(
      '#method'   => 'bitcache.get',
      '#callback' => 'bitcache_get_contents',
      '#return'   => 'base64',
      '#args'     => array(
        array('#name' => 'id', '#type' => 'string', '#optional' => FALSE, '#description' => t('The bitstream\'s identifier (SHA-1 fingerprint).')),
      ),
      '#help'     => t('Retrieves the contents of the given bitstream.'),
    ),
    array(
      '#method'   => 'bitcache.put',
      '#callback' => 'bitcache_service_put',
      '#return'   => 'boolean',
      '#args'     => array(
        array('#name' => 'id',   '#type' => 'string', '#optional' => FALSE, '#description' => t('The bitstream\'s identifier (SHA-1 fingerprint).')),
        array('#name' => 'data', '#type' => 'base64', '#optional' => FALSE, '#description' => t('The bitstream\'s contents.')),
        array('#name' => 'repo', '#type' => 'string', '#optional' => TRUE,  '#description' => t('The Bitcache repository to store the bitstream in.')),
      ),
      '#help'     => t('Stores the contents of the given bitstream.'),
    ),
    array(
      '#method'   => 'bitcache.delete',
      '#callback' => 'bitcache_delete',
      '#return'   => 'boolean',
      '#args'     => array(
        array('#name' => 'id', '#type' => 'string', '#optional' => FALSE, '#description' => t('The bitstream\'s identifier (SHA-1 fingerprint).')),
      ),
      '#help'     => t('Deletes the given bitstream.'),
    ),
  );
}

function bitcache_service_put($id, $data, $repo = NULL) {
  if (!empty($repo)) {
    bitcache_use_repository($repo);
  }
  return bitcache_put($id, $data);
}
