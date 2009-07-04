<?php
// $Id: bitcache.token.inc,v 1.1 2009/07/01 04:09:48 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Token API hooks

/**
 * Implementation of hook_token_list().
 */
function bitcache_token_list($type = 'all') {
  $tokens = array();
  $tokens['global']['file-directory-path'] = t('The file system path for the current Drupal website.');
  $tokens['global']['file-directory-temp'] = t('The temporary directory for the current Drupal website.');
  $tokens['global']['site-host']           = t('The HTTP host name of the current Drupal website.');
  return $tokens;
}

/**
 * Implementation of hook_token_values().
 */
function bitcache_token_values($type, $object = NULL) {
  $values = array();
  switch ($type) {
    case 'global':
      $values['file-directory-path'] = file_directory_path();
      $values['file-directory-temp'] = file_directory_temp();
      $values['site-host']           = $_SERVER['HTTP_HOST'];
      break;
  }
  return $values;
}
