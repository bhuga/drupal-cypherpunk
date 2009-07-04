<?php
// $Id: lockdown.token.inc,v 1.1 2009/06/25 01:15:20 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Token API hooks

/**
 * Implementation of hook_token_list().
 */
function lockdown_token_list($type = 'all') {
  $tokens = array();
  switch ($type) {
    case 'all':
    case 'lockdown':
      $tokens['lockdown']['lockdown-level-name'] = t('The machine-readable name of the current lockdown level.');
      $tokens['lockdown']['lockdown-level-title'] = t('The human-readable title of the current lockdown level.');
      $tokens['lockdown']['lockdown-level-description'] = t('The description of the current lockdown level.');
      break;
  }
  return $tokens;
}

/**
 * Implementation of hook_token_values().
 */
function lockdown_token_values($type, $object = NULL, $options = array()) {
  $values = array();
  switch ($type) {
    case 'lockdown':
      if ($level = lockdown_get_level_info()) {
        foreach (array('name', 'title', 'description') as $property) {
          $values['lockdown-level-' . $property] = $level->$property;
        }
      }
      break;
  }
  return $values;
}
