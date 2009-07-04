<?php
// $Id: lockdown.messaging.inc,v 1.1 2009/06/25 01:15:20 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Messaging API hooks

/**
 * Implementation of hook_messaging().
 */
function lockdown_messaging($op, $type = NULL) {
  switch ($op) {
    case 'message types':
      return array(
        'lockdown' => array(
          'name'        => t('Lockdown messages'),
          'description' => t('Messages triggered by Lockdown and security events.')
        ),
      );

    case 'message groups':
      return array(
        'lockdown-alert' => array(
          'module'      => 'lockdown',
          'name'        => t('Lockdown alert'),
          'description' => t('Defaults for all alerts related to Lockdown events.'),
        ),
        'lockdown-alert-disable' => array(
          'module'      => 'lockdown',
          'name'        => t('Lockdown disabled'),
          'description' => t('Alert triggered when the Lockdown module is disabled.'),
          'help'        => $help = t('The <em>Header</em> and <em>Footer</em> will be taken from <a href="@alerts"><em>Lockdown alert</em></a>.', array('@alerts' => url('admin/messaging/template/edit/lockdown-alert'))),
          'fallback'    => 'lockdown-alert',
        ),
        'lockdown-alert-level' => array(
          'module'      => 'lockdown',
          'name'        => t('Lockdown level changed'),
          'description' => t('Alert triggered when the lockdown level is changed.'),
          'help'        => $help,
          'fallback'    => 'lockdown-alert',
        ),
      );

    case 'message keys':
      switch ($type) {
        case 'lockdown-alert':
          return array(
            'subject' => t('Subject'),
            'header'  => t('Header'),
            'main'    => t('Content'),
            'footer'  => t('Footer'),
          );

        case 'lockdown-alert-disable':
        case 'lockdown-alert-level':
          return array(
            'subject' => t('Subject'),
            'main'    => t('Content'),
          );
      }
      break;

    case 'messages':
      switch ($type) {
        case 'lockdown-alert':
          return array(
            'subject' => t('[[site-name]] Lockdown alert'),
            'header'  => t('This is a security alert from [site-url]:'),
            'main'    => array(),
            'footer'  => array(),
          );

        case 'lockdown-alert-disable':
          return array(
            'subject' => t('[[site-name]] Lockdown module disabled'),
            'main'    => t('The Lockdown module was disabled by [user-name].'),
          );

        case 'lockdown-alert-level':
          return array(
            'subject' => t('[[site-name]] Lockdown level changed to "[lockdown-level-title]"'),
            'main'    => t('The lockdown level was changed to "[lockdown-level-title]" by [user-name].'),
          );
      }
      break;

    case 'tokens':
      return array('lockdown');
  }
}

//////////////////////////////////////////////////////////////////////////////
// Messaging API helpers

/**
 * Sends an alert to the administrator.
 */
function lockdown_alert_admin($op, $objects = array()) {
  global $user;

  $objects = array_merge(array('lockdown' => NULL, 'user' => $user), $objects);
  $message = lockdown_alert_message($op, $objects);
  foreach (lockdown_get_admins() as $recipient) {
    lockdown_alert_send($recipient, $message);
  }
}

/**
 * Composes a message object.
 */
function lockdown_alert_message($group = NULL, $objects = array()) {
  $message = array('type' => 'lockdown', 'subject' => '', 'body' => array());
  $message['subject'] = lockdown_alert_message_part($group, 'subject', $objects);
  foreach (array('header', 'main', 'footer') as $key) {
    $message['body'][$key] = lockdown_alert_message_part($group, $key, $objects);
  }
  return (object)$message;
}

/**
 * Composes a message part, taking care of token substitution and such.
 */
function lockdown_alert_message_part($group, $key, $objects = NULL) {
  $group = !$group ? array() : (is_array($group) ? $group : array($group));
  $group = array_merge(array('lockdown', 'alert'), $group);
  while (!empty($group)) {
    if (($text = messaging_message_part(implode('-', $group), $key))) {
      $text = ($text == MESSAGING_EMPTY) ? '' : $text;
      break;
    }
    array_pop($group);
  }
  return !$objects ? $text : messaging_text_replace($text, $objects);
}

/**
 * Sends a message to a user via the Messaging API.
 */
function lockdown_alert_send($account, $message, $method = NULL) {
  return messaging_message_send_user($account, $message, $method);
}
