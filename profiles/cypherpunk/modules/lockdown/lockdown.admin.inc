<?php
// $Id: lockdown.admin.inc,v 1.8 2009/06/29 02:24:31 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Lockdown level block

function lockdown_admin_block_configure() {
  $form = array();

  $form['lockdown'] = array('#tree' => TRUE);
  $form['lockdown']['title'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Display lockdown level title'),
    '#default_value' => (int)variable_get('lockdown_level_block[title]', TRUE),
  );
  $form['lockdown']['description'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Display lockdown level description'),
    '#default_value' => (int)variable_get('lockdown_level_block[description]', TRUE),
  );

  return $form;
}

function lockdown_admin_block_submit($edit) {
  variable_set('lockdown_level_block[title]', $edit['lockdown']['title']);
  variable_set('lockdown_level_block[description]', $edit['lockdown']['description']);
}

function theme_lockdown_level_block($level) {
  $output = array();
  if (variable_get('lockdown_level_block[title]', TRUE)) {
    $output[] = '<span class="lockdown level" id="block-lockdown-level-title">';
    $output[] = '<strong>' . $level->title;
    $output[] = variable_get('lockdown_level_block[description]', TRUE) ? ':&nbsp;' : '';
    $output[] = '</strong>';
    $output[] = '</span>';
  }
  if (variable_get('lockdown_level_block[description]', TRUE)) {
    $output[] = '<span class="lockdown level" id="block-lockdown-level-description">';
    $output[] = $level->description;
    $output[] = '</span>';
  }
  return implode('', $output);
}

//////////////////////////////////////////////////////////////////////////////
// Lockdown data retention block

function theme_lockdown_data_retention_block($period = NULL) {
  return is_int($period) ?
    t('This site retains visitor IP addresses for a maximum period of @period.', array('@period' => format_interval($period))) :
    t('This site retains visitor IP addresses for an indefinite period.');
}

//////////////////////////////////////////////////////////////////////////////
// Lockdown settings

function lockdown_admin_settings() {
  $form = array('#submit' => array('lockdown_admin_settings_submit'));

  // Current lockdown level
  $form['level'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Current lockdown level'),
    '#collapsible'   => FALSE,
    '#description'   => t(''), // TODO
  );
  $form['level']['lockdown_level'] = array(
    '#type'          => 'radios',
    '#title'         => NULL,
    '#default_value' => $level  = lockdown_get_level(),
    '#options'       => $levels = lockdown_get_levels('title'),
    '#description'   => t('Only users with the <em>administer lockdown</em> privilege can change this setting.'),
    '#disabled'      => !user_access('administer lockdown') || count($levels) < 2,
  );
  $form['level']['submit'] = array('#type' => 'submit', '#value' => t('Activate lockdown level'));

  // Emphasize the current lockdown level:
  $form['level']['lockdown_level']['#options'][$level] = '<strong>' . $form['level']['lockdown_level']['#options'][$level] . '</strong>';

  // Current lockdown configuration
  $form['options'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Current lockdown configuration'),
    '#collapsible'   => FALSE,
    '#description'   => t(''), // TODO
    '#value'         => lockdown_admin_settings_overrides($level),
  );

  // Current lockdown rules
  $form['rules'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Current lockdown rules'),
    '#collapsible'   => FALSE,
    '#description'   => t(''), // TODO
    '#value'         => lockdown_admin_settings_rules($level),
  );

  return $form;
}

function theme_lockdown_admin_settings($form) {
  return drupal_render($form);
}

/**
 * Table: Current lockdown configuration.
 */
function lockdown_admin_settings_overrides($level) {
  $head = array(array('data' => t('Setting'), 'width' => '30%'), array('data' => t('Value'), 'width' => '50%'), t('Operations'));
  $rows = array();

  $level  = lockdown_get_level_info($level);
  $config = lockdown_admin_level_form($form_state = array(), $level);
  foreach (array_intersect_key($config['config'], array_flip(element_children($config['config']))) as $fieldset) {
    if (empty($fieldset['override']['#default_value'])) continue;

    // Section
    $rows[] = array(
      'data'  => array(array('data' => $fieldset['#title'], 'colspan' => 3, 'class' => 'region')),
      'class' => 'region',
    );

    // Variables
    foreach (array_intersect_key($fieldset['variables'], array_flip(element_children($fieldset['variables']))) as $variable_id => $variable) {
      $value = $variable['#default_value'];
      if (!isset($level->options[$variable_id]) || $value === '' || !empty($variable['#hidden'])) continue;

      $title = '&nbsp;&nbsp;' . $variable['#title'];
      $value = isset($variable['#options']) ? $variable['#options'][$value] : $value;
      $link  = l(t('edit'), 'admin/settings/lockdown/levels/edit/' . $level->name, array('query' => array('destination' => $_GET['q'])));

      $rows[] = array('data' => array($title, $value, $link), 'valign' => 'top');
    }

    if (!element_children($fieldset['variables'])) {
      $rows[] = array(array('data' => t('No overridden settings.'), 'colspan' => 3, 'class' => 'region-empty'));
    }
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No overridden settings <a href="@edit">configured</a>.', array('@edit' => url('admin/settings/lockdown/levels/edit/' . $level->name, array('query' => array('destination' => $_GET['q']))))), 'colspan' => 3, 'class' => 'region-empty'));
  }

  return theme('table', $head, $rows, array('id' => 'lockdown-levels', 'class' => 'lockdown levels'));
}

/**
 * Table: Current lockdown rules.
 */
function lockdown_admin_settings_rules($level) {
  if (!module_exists('rules')) {
    return '<small>' . t('This functionality requires the <a href="@rules" target="_blank">Rules</a> module.', array('@rules' => 'http://drupal.org/project/rules')) . '</small>';
  }

  $head = array(array('data' => t('Rule'), 'width' => '80%'), t('Operations'));
  $rows = array();

  foreach (lockdown_get_rules_for_set('lockdown_level_' . $level) as $rule_id => $rule) {
    if (module_exists('rules_admin')) {
      $rule = rule_load($rule_id)->get_indexed_rule();
    }

    // Rule
    $rows[] = array(
      'data'  => array(
        array('data' => $rule['#label'], 'class' => 'region'),
        array('data' => l(t('edit'), 'admin/rules/rules/' . $rule_id, array('query' => array('destination' => $_GET['q']), 'attributes' => array('style' => 'font-weight: normal;'))), 'class' => 'region'),
      ),
      'class' => 'region',
    );

    // Actions
    unset($rule['#actions']['#id']);
    foreach ($rule['#actions'] as $action) {
      $rows[] = array(
        'data'    => array(
          '&nbsp;&nbsp;' . $action['#info']['label'],
          !isset($action['#id']) ? '-' : l(t('edit'), 'admin/rules/rules/' . $rule_id . '/edit/' . $action['#id'], array('query' => array('destination' => $_GET['q']))),
         ),
         'valign' => 'top',
      );
    }
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No rules in the <a href="@edit">rule set</a>.', array('@edit' => url('admin/rules/rule_sets/lockdown_level_' . $level . '/edit'))), 'colspan' => 3, 'class' => 'region-empty'));
  }

  return theme('table', $head, $rows, array('id' => 'lockdown-rules', 'class' => 'lockdown rules'));
}

function lockdown_admin_settings_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (!user_access('administer lockdown')) {
    unset($form_state['values']['lockdown_level']);
  }
}

function lockdown_admin_settings_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (user_access('administer lockdown')) {
    lockdown_set_level($lockdown_level);
    drupal_set_message(t('The lockdown level %title has been activated.', array('%title' => lockdown_get_level_info()->title)));
  }
}

//////////////////////////////////////////////////////////////////////////////
// Lockdown levels

function lockdown_admin_levels() {
  $form = array('#tree' => TRUE, '#theme' => 'lockdown_admin_levels', '#submit' => array('lockdown_admin_levels_submit'));

  // Lockdown level table
  foreach (lockdown_get_levels() as $name => $level) {
    $level = (object)$level;
    $form[$name]['title']       = array('#value' => l($level->title, 'admin/settings/lockdown/levels/edit/' . $name));
    $form[$name]['description'] = array('#value' => $level->description);
    $form[$name]['weight']      = array('#type' => 'weight', '#delta' => 10, '#default_value' => $level->weight);
    $form[$name]['edit']        = array('#value' => l(t('configure'), 'admin/settings/lockdown/levels/edit/' . $name));
    $form[$name]['delete']      = array('#value' => l(t('delete'), 'admin/settings/lockdown/levels/delete/' . $name));

    if ($name == lockdown_get_level()) {
      $form[$name]['title']['#value'] = '<strong>' . $form[$name]['title']['#value'] . '</strong>';
      $form[$name]['delete']['#value'] = '';
    }
  }

  // Create button
  $form['new']['title'] = array(
    '#type'          => 'textfield',
    '#maxlength'     => 255,
    '#attributes'    => array('style' => 'width: 95%;'),
  );
  $form['new']['create'] = array('#type' => 'submit', '#value' => t('Create'));

  // Save & Reset buttons
  $form['buttons'] = array('#weight' => 99);
  $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));
  $form['buttons']['reset']  = array('#type' => 'submit', '#value' => t('Reset to defaults'));

  return $form;
}

function theme_lockdown_admin_levels($form) {
  $head = array(array('data' => t('Name'), 'width' => '30%'), t('Description'), t('Weight'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array();

  foreach (element_children($form) as $key) {
    if (!isset($form[$key]['description'])) {
      continue;
    }

    $row = &$form[$key];
    $row['weight']['#attributes']['class'] = 'level-weight';
    $rows[] = array(
      'data' => array(
        drupal_render($row['title']),
        drupal_render($row['description']),
        drupal_render($row['weight']),
        drupal_render($row['edit']),
        drupal_render($row['delete']),
      ),
      'class' => 'draggable',
    );
  }

  $rows[] = array(
    array('data' => drupal_render($form['new']['title']), 'colspan' => 3),
    array('data' => drupal_render($form['new']['create']), 'colspan' => 2),
  );

  $form['#value'] = theme('table', $head, $rows, array('id' => 'lockdown-levels', 'class' => 'lockdown levels'));
  drupal_add_tabledrag('lockdown-levels', 'order', 'sibling', 'level-weight');

  return drupal_render($form);
}

function lockdown_admin_levels_submit($form, &$form_state) {
  // Create new lockdown level
  if ($form_state['clicked_button']['#value'] == t('Create')) {
    $form_state['redirect'] = array('admin/settings/lockdown/levels/create', drupal_query_string_encode(array('title' => $form_state['values']['new']['title'])));
    return;
  }

  // Reset lockdown level weights
  if ($form_state['clicked_button']['#value'] == t('Reset to defaults')) {
    db_query("UPDATE {lockdown_levels} SET weight = 0");
    db_query("UPDATE {lockdown_levels} SET weight = -1 WHERE name = '%s'", 'online');
    drupal_set_message(t('The configuration options have been reset to their default values.'));
    return;
  }

  // Update lockdown level weights
  unset($form_state['values']['new']);
  foreach ($form_state['values'] as $name => $row) {
    if (isset($row['weight'])) {
      db_query("UPDATE {lockdown_levels} SET weight = %d WHERE name = '%s'", $row['weight'], $name);
    }
    unset($form_state['values'][$name]); // don't save anything to the {variable} table
  }
  drupal_set_message(t('The configuration options have been saved.'));
}

//////////////////////////////////////////////////////////////////////////////
// Lockdown level settings

function lockdown_admin_level_create() {
  return drupal_get_form('lockdown_admin_level_form', array('name' => @$_GET['name'], 'title' => @$_GET['title'], 'description' => '', 'options' => array()));
}

function lockdown_admin_level_edit($name) {
  return drupal_get_form('lockdown_admin_level_form', lockdown_get_level_info($name));
}

function lockdown_admin_level_form(&$form_state, $edit = array('name' => '', 'title' => '', 'description' => '', 'options' => array())) {
  $edit = (object)$edit;
  $form = array();

  drupal_add_js(drupal_get_path('module', 'lockdown') . '/lockdown.admin.js', 'module');

  // Lockdown level
  $form['identity'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Lockdown level'),
    '#collapsible'   => TRUE,
    '#collapsed'     => !empty($edit->name),
    '#weight'        => -50,
  );
  $form['identity']['title'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Title'),
    '#default_value' => $edit->title,
    '#maxlength'     => 255,
    '#description'   => t('The human-readable name of this lockdown level.'),
    '#required'      => TRUE,
  );
  $form['identity']['name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Name'),
    '#default_value' => $edit->name,
    '#maxlength'     => 64,
    '#description'   => t('The machine-readable name of this lockdown level. This name must contain only lowercase letters, numbers, and underscores. The name must be unique.'),
    '#required'      => TRUE,
  );
  $form['identity']['description'] = array(
    '#title'         => t('Description'),
    '#type'          => 'textarea',
    '#default_value' => $edit->description,
    '#rows'          => 2,
    '#description'   => t('A brief description of this lockdown level. This is only used for administrative purposes, so you can leave this empty if you wish.'),
  );

  $form['config'] = array('#tree' => TRUE);

  // Site maintenance status
  $form['config']['maintenance'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Site maintenance status'),
    '#collapsible'   => TRUE,
    '#collapsed'     => !($overridden = isset($edit->options['site_offline']) || isset($edit->options['site_offline_message'])),
    '#description'   => t('Configure whether this site will be put in maintenance mode when this lockdown level is activated.'),
  );
  $form['config']['maintenance']['override'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Override site defaults'),
    '#default_value' => $overridden,
    '#attributes'    => array('class' => 'lockdown-override'),
  );
  $form['config']['maintenance']['variables'] = array(
    '#prefix'        => '<div id="edit-config-maintenance-variables">',
    '#suffix'        => '</div>',
  );
  lockdown_site_maintenance_settings($form['config']['maintenance']['variables'], $edit->options);

  // Transport security policy
  $form['config']['transport'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Transport security policy'),
    '#collapsible'   => TRUE,
    '#collapsed'     => !($overridden = isset($edit->options['securepages_secure'])),
    '#description'   => t('Configure the <a href="http://en.wikipedia.org/wiki/Transport_Layer_Security" target="_blank">transport-level security policy</a> to enforce when this lockdown level is active.'),
  );
  $form['config']['transport']['override'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Override site defaults'),
    '#default_value' => $overridden,
    '#attributes'    => array('class' => 'lockdown-override'),
    '#description'   => '<small>' . t('This functionality requires the <a href="http://drupal.org/project/securepages" target="_blank">Secure Pages</a> module.') . '</small>',
    '#disabled'      => !module_exists('securepages'),
  );
  $form['config']['transport']['variables'] = array(
    '#prefix'        => '<div id="edit-config-transport-variables">',
    '#suffix'        => '</div>',
  );
  lockdown_transport_security_settings($form['config']['transport']['variables'], $edit->options);

  // Messaging security policy
  $form['config']['messaging'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Messaging security policy'),
    '#collapsible'   => TRUE,
    '#collapsed'     => !($overridden = isset($edit->options['openpgp_mail_encrypt'])),
    '#description'   => t('Configure the <a href="http://en.wikipedia.org/wiki/E-mail_encryption" target="_blank">e-mail encryption policy</a> to enforce when this lockdown level is active.'),
  );
  $form['config']['messaging']['override'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Override site defaults'),
    '#default_value' => $overridden,
    '#attributes'    => array('class' => 'lockdown-override'),
    '#description'   => '<small>' . t('This functionality requires the <a href="http://drupal.org/project/openpgp" target="_blank">OpenPGP</a> module.') . '</small>',
    '#disabled'      => !module_exists('openpgp'),
  );
  $form['config']['messaging']['variables'] = array(
    '#prefix'        => '<div id="edit-config-messaging-variables">',
    '#suffix'        => '</div>',
  );
  lockdown_messaging_security_settings($form['config']['messaging']['variables'], $edit->options);

  // Data retention policy
  $form['config']['retention'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Data retention policy'),
    '#collapsible'   => TRUE,
    '#collapsed'     => !($overridden = isset($edit->options['ip_anon_policy'])),
    '#description'   => t('Configure the <a href="http://en.wikipedia.org/wiki/Telecommunications_data_retention" target="_blank">data retention</a> policy to enforce when this lockdown level is active.'),
  );
  $form['config']['retention']['override'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Override site defaults'),
    '#default_value' => $overridden,
    '#attributes'    => array('class' => 'lockdown-override'),
    '#description'   => '<small>' . t('This functionality is compatible with the <a href="http://drupal.org/project/ip_anon" target="_blank">IP Anonymize</a> module (but does not require it).') . '</small>',
  );
  $form['config']['retention']['variables'] = array(
    '#prefix'        => '<div id="edit-config-retention-variables">',
    '#suffix'        => '</div>',
  );
  lockdown_data_retention_settings($form['config']['retention']['variables'], $edit->options);

  $form['key']    = array('#type' => 'hidden', '#value' => $edit->name);
  $form['submit'] = array('#type' => 'submit', '#value' => empty($edit->name) ? t('Create lockdown level') : t('Update lockdown level'));
  return $form;
}

/**
 * Overrides: Site maintenance.
 *
 * @see system_site_maintenance_settings()
 */
function lockdown_site_maintenance_settings(&$form, $edit) {
  $form['site_offline'] = array(
    '#type'          => 'radios',
    '#title'         => t('Site status'),
    '#default_value' => isset($edit['site_offline']) ? $edit['site_offline'] : variable_get('site_offline', 0),
    '#options'       => array(t('Online'), t('Off-line')),
    '#description'   => t('When set to "Online", all visitors will be able to browse your site normally. When set to "Off-line", only users with the "administer site configuration" permission will be able to access your site to perform maintenance; all other visitors will see the site off-line message configured below. Authorized users can log in during "Off-line" mode directly via the <a href="@user-login">user login</a> page.', array('@user-login' => url('user'))),
  );

  $form['site_offline_message'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Site off-line message'),
    '#default_value' => isset($edit['site_offline_message']) ? $edit['site_offline_message'] : variable_get('site_offline_message', t('@site is currently under maintenance. We should be back shortly. Thank you for your patience.', array('@site' => lockdown_variable_get('site_name', 'Drupal')))),
    '#description'   => t('Message to show visitors when the site is in off-line mode.')
  );

  return $form;
}

/**
 * Overrides: Transport security policy.
 */
function lockdown_transport_security_settings(&$form, $edit) {
  $form['securepages_secure'] = array(
    '#type'          => 'radios',
    '#title'         => t('Pages which will be be secure'),
    '#default_value' => isset($edit['securepages_secure']) ? $edit['securepages_secure'] : variable_get('securepages_secure', 1),
    '#options'       => array(t('Make secure every page except the listed pages.'), t('Make secure only the listed pages.')),
  );
  $form['securepages_pages'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Pages'),
    '#default_value' => isset($edit['securepages_pages']) ? $edit['securepages_pages'] : variable_get('securepages_pages', "node/add*\nnode/*/edit\nuser/*\nadmin*"),
    '#rows'          => 5,
    '#description'   => t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are '<em>blog</em>' for the blog page and '<em>blog/*</em>' for every personal blog. '<em>&lt;front&gt;</em>' is the front page."),
  );

  return $form;
}

/**
 * Overrides: Messaging security policy.
 */
function lockdown_messaging_security_settings(&$form, $edit) {
  if (module_exists('openpgp')) {
    foreach (module_invoke('openpgp', 'lockdown_settings', 'form', 'messaging') as $variable => $variable_form) {
      if (isset($edit[$variable])) {
        $variable_form['#default_value'] = $edit[$variable];
      }
      $form[$variable] = $variable_form;
    }
  }

  return $form;
}

/**
 * Overrides: Data retention policy.
 */
function lockdown_data_retention_settings(&$form, $edit) {
  $form['#theme'] = 'lockdown_data_retention_settings';

  $form['ip_anon_policy'] = array(
    '#type'          => 'radios',
    '#title'         => t('IP address retention policy'),
    '#default_value' => isset($edit['ip_anon_policy']) ? $edit['ip_anon_policy'] : variable_get('ip_anon_policy', 0),
    '#options'       => array(t('Retain IP addresses'), t('Scrub IP addresses periodically')),
    '#description'   => t('Select whether to anonymize IP addresses on each cron run according to the retention periods configured below. Anonymization is done by overwriting IP addresses older than the retention period with the pseudo-address \'0.0.0.0\'.'),
  );

  $periods = drupal_map_assoc(array(0, 30, 60, 120, 180, 300, 600, 900, 1800, 2700, 3600, 5400, 7200, 10800, 21600, 32400, 43200, 64800, 86400, 172800, 259200, 345600, 604800, 1209600, 2419200, 4838400, 9676800, 31536000), 'format_interval');
  foreach (lockdown_get_ip_anon_tables() as $table => $columns) {
    $setting = 'ip_anon_period_' . $table;
    $form[$setting] = array(
      '#type'          => 'select',
      '#title'         => '&nbsp;&nbsp;' . t('@table table', array('@table' => $table)), // NOTE: displayed only at <admin/settings/lockdown> summary
      '#options'       => $periods,
      '#default_value' => isset($edit[$setting]) ? $edit[$setting] : variable_get($setting, 10800),
      '#description'   => NULL,
      '#hidden'        => empty($edit['ip_anon_policy']), // NOTE: used for <admin/settings/lockdown> summary
    );
  }

  return $form;
}

function theme_lockdown_data_retention_settings($form) {
  $head = array(array('data' => t('Table'), 'width' => '20%'), array('data' => t('Description'), 'width' => '60%'), t('Period'));
  $rows = array();

  foreach (lockdown_get_ip_anon_tables() as $table => $columns) {
    $schema = drupal_get_schema($table);

    unset($form['ip_anon_period_' . $table]['#title']);
    $rows[] = array(
      array('data' => $table, 'valign' => 'top'),
      array('data' => $schema['description'], 'valign' => 'top'),
      array('data' => drupal_render($form['ip_anon_period_' . $table]), 'valign' => 'top'),
    );
  }

  $form['ip_anon_table'] = array(
    '#prefix' => '<div id="edit-config-retention-ip-table" style="' . (empty($form['ip_anon_policy']['#default_value']) ? 'display: none;' : '') . '">',
    '#value'  => theme('table', $head, $rows, array('class' => 'lockdown table')),
    '#suffix' => '</div>',
  );
  return drupal_render($form);
}

function lockdown_admin_level_form_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  $options = array();
  foreach ($config as $fieldset) {
    foreach ($fieldset['variables'] as $var_name => $var_value) {
      if ($fieldset['override']) {
        $options[$var_name] = $var_value;
      }
    }
  }

  if (empty($key)) {
    db_query("INSERT INTO {lockdown_levels} (name, title, description, weight, options) VALUES ('%s', '%s', '%s', %d, '%s')", $name, $title, $description, 0, serialize($options));
    drupal_set_message(t('The lockdown level %title has been created.', array('%title' => $title)));
  }
  else {
    db_query("UPDATE {lockdown_levels} SET name = '%s', title = '%s', description = '%s', options = '%s' WHERE name = '%s'", $name, $title, $description, serialize($options), $key);
    if ($key == lockdown_get_level()) {
      lockdown_set_level($name); // update any changed system settings if this lockdown level is active currently
    }
    drupal_set_message(t('The lockdown level %title has been updated.', array('%title' => $title)));
  }

  $form_state['redirect'] = 'admin/settings/lockdown/levels';
}

function lockdown_admin_level_delete($form_state, $key) {
  if (($level = lockdown_get_level_info($key))) {
    $form['key'] = array('#type' => 'value', '#value' => $level->name);

    return confirm_form($form,
      t('Are you sure you want to delete the lockdown level %title?', array('%title' => $level->title)),
      isset($_GET['destination']) ? $_GET['destination'] : 'admin/settings/lockdown/levels',
      t('This action cannot be undone.'));
  }
}

function lockdown_admin_level_delete_submit($form, &$form_state) {
  if (($level = lockdown_get_level_info($form_state['values']['key']))) {
    if ($form_state['values']['confirm']) {
      db_query("DELETE FROM {lockdown_levels} WHERE name = '%s'", $level->name);
      drupal_set_message(t('The lockdown level %title has been deleted.', array('%title' => $level->title)));
    }
  }

  $form_state['redirect'] = 'admin/settings/lockdown/levels';
}
