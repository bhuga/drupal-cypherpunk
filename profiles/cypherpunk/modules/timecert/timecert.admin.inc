<?php
// $Id: timecert.admin.inc,v 1.1 2009/05/16 22:31:43 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Settings form

function timecert_admin_settings() {
  $form = array();

  // Content type settings
  $form['node_types'] = array('#type' => 'fieldset', '#title' => t('Content type settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['node_types']['timecert_node_types'] = array(
    '#type'          => 'checkboxes',
    '#title'         => t('Timestamped content types'),
    '#default_value' => timecert_get_node_types(),
    '#options'       => node_get_types('names'),
    '#description'   => t('Select which types of content you wish to timestamp and show the TimeCert certificate for.'),
  );

  // Digest settings
  $form['digest'] = array('#type' => 'fieldset', '#title' => t('Timestamping settings'), '#collapsible' => TRUE, '#collapsed' => TRUE);
  $form['digest']['timecert_digest_type'] = array(
    '#type'          => 'radios',
    '#title'         => t('Content digest type'),
    '#default_value' => variable_get('timecert_digest_type', 'url'),
    '#options'       => array('url' => t('Timestamp the URL, providing evidence that a page existed at a given time')),
    '#description'   => t(''),
    '#disabled'      => TRUE,
  );
  $form['digest']['timecert_bitcache_repository'] = array(
    '#type'          => 'select',
    '#title'         => t('Store digest input in Bitcache'),
    '#default_value' => variable_get('timecert_bitcache_repository', ''),
    '#options'       => array_merge(array('' => t('<select repository>')), module_exists('bitcache') ? bitcache_get_repositories('titles') : array()),
    '#description'   => t('For evidential and verification purposes it may be useful to keep an easily-accessible permanent archive of the input data to the timestamped digest. This requires the <a href="http://drupal.org/project/bitcache">Bitcache</a> module.'),
  );
  if (!module_exists('bitcache')) {
    $form['digest']['timecert_bitcache_repository']['#disabled'] = TRUE;
  }

  // Timestamping throttle
  $form['throttle'] = array('#type' => 'fieldset', '#title' => t('Timestamping throttle'), '#collapsible' => TRUE, '#collapsed' => TRUE);
  $form['throttle']['timecert_cron_limit'] = array(
    '#type'          => 'select',
    '#title'         => t('Number of items to timestamp per cron run'),
    '#default_value' => variable_get('timecert_cron_limit', 50),
    '#options'       => drupal_map_assoc(array(10, 20, 50, 100, 200, 500)),
    '#description'   => t('The maximum number of already existing content items that will be timestamped in each pass of a <a href="@cron">cron maintenance task</a>. If necessary, reduce the number of items to prevent timeouts of the cron job.', array('@cron' => url('admin/reports/status'))),
  );

  return system_settings_form($form);
}
