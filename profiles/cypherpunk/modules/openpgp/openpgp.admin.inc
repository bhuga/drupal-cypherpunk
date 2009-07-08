<?php
// $Id: openpgp.admin.inc,v 1.1 2009/07/08 18:17:33 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Settings form

function openpgp_admin_settings() {
  $form = array();

  module_load_include('inc', 'openpgp', 'openpgp');

  // Public key storage
  $form['key'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Public key storage'),
    '#collapsible'   => TRUE,
    '#collapsed'     => openpgp_get_profile_field_name(),
    '#description'   => t(''),
  );
  $form['key']['openpgp_profile_field'] = array(
    '#type'          => 'select',
    '#title'         => t('Profile field for public keys'),
    '#default_value' => openpgp_get_profile_field_name(),
    '#options'       => array_merge(array('' => t('<none>')), openpgp_get_profile_fields()),
    '#required'      => FALSE,
    '#description'   => t('Select the custom <a href="@profile-fields">profile field</a> in which users\' public keys are stored. Note that only multi-line textfields can be used for storing public keys, so no other types of profile fields are displayed or can be selected for this purpose.', array('@profile-fields' => url('admin/user/profile'))),
  );

  // E-mail encryption settings
  $form['mail'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('E-mail encryption settings'),
    '#collapsible'   => TRUE,
    '#collapsed'     => ($disabled = (count(openpgp_get_providers()) == 0)),
    '#description'   => $disabled ? ('<strong>' . t('This functionality requires that the GnuPG submodule be enabled.') . '</strong>') : '',
  );
  $form['mail']['openpgp_mail_encrypt'] = array(
    '#type'          => 'radios',
    '#title'         => t('Encrypt outgoing e-mail'),
    '#default_value' => $disabled ? '' : variable_get('openpgp_mail_encrypt', variable_get('gnupg_mail_encrypt', '')),
    '#options'       => array('' => t('Never'), 'custom' => t('Optionally, when possible'), 'always' => t('Always, when possible')),
    '#required'      => FALSE,
    '#description'   => t('Whether to encrypt Drupal\'s outgoing e-mails using the public key of the recipient(s). This is only possible in those cases when the public keys for all of the message\'s recipients are <a href="@public-keys">on file</a>. When "Optionally" is selected, users themselves can choose, in their user profile, whether they wish to receive encrypted e-mail.', array('@public-keys' => url('admin/user/openpgp'))),
    '#disabled'      => $disabled,
  );
  $form['mail']['openpgp_mail_comment'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Encrypted e-mail comment'),
    '#default_value' => variable_get('openpgp_mail_comment', variable_get('gnupg_mail_comment', '')),
    '#maxlength'     => 60,
    '#description'   => t('Enter text to use as a comment string in plaintext signatures and ASCII-armored messages. Keep the length below 60 characters to avoid problems with mail programs wrapping such lines. The default behavior is to not use a comment string.'),
    '#disabled'      => $disabled,
  );
  $form['mail']['openpgp_mail_header'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Encrypted e-mail header'),
    '#default_value' => variable_get('openpgp_mail_header', variable_get('gnupg_mail_header', '')),
    '#rows'          => 2,
    '#description'   => t('Enter text to include in the e-mail message prior to the start of the actual encrypted message. This plaintext will be prepended immediately in front of the <tt>BEGIN PGP MESSAGE</tt> marker. By default no header text will be used.'),
    '#disabled'      => $disabled,
  );
  $form['mail']['openpgp_mail_footer'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Encrypted e-mail footer'),
    '#default_value' => variable_get('openpgp_mail_footer', variable_get('gnupg_mail_footer', '')),
    '#rows'          => 2,
    '#description'   => t('Enter text to include in the e-mail message subsequent to the end of the actual encrypted message. This plaintext will be appended immediately after the <tt>END PGP MESSAGE</tt> marker. By default no footer text will be used.'),
    '#disabled'      => $disabled,
  );

  return system_settings_form($form);
}

function openpgp_admin_settings_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);
}

//////////////////////////////////////////////////////////////////////////////
// Public keys management

function openpgp_admin_users() {
  $head = array(t('Username'), t('Key ID'), t('Key created'), t('Operations'));
  $rows = array();

  if (!($field = openpgp_get_profile_field())) {
    drupal_set_message(t('The OpenPGP module has not been <a href="@openpgp">configured</a> yet.', array('@openpgp' => url('admin/settings/openpgp'))), 'error');
    return '';
  }

  $result = pager_query("SELECT pv.uid FROM {profile_values} pv LEFT JOIN {users} u ON pv.uid = u.uid LEFT JOIN {profile_fields} pf ON pv.fid = pf.fid WHERE pf.name = '%s' AND pv.value != '' ORDER BY u.name ASC", 20, 0, NULL, $field->name);
  while ($account = db_fetch_object($result)) {
    $key = openpgp_get_user_key($account = user_load($account->uid));
    $rows[] = array(
      l($account->name, 'user/' . $account->uid),
      empty($key->key_id)    ? t('n/a') : '<acronym' . drupal_attributes(array('title' => strtoupper($key->fingerprint))) . '>' . check_plain(strtoupper($key->key_id)) . '</acronym>',
      empty($key->timestamp) ? t('n/a') : format_date($key->timestamp, 'custom', 'Y-m-d'),
      l(t('edit'), 'user/' . $account->uid . '/edit/' . $field->category, array('query' => array('destination' => $_GET['q']))),
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No public keys stored by users.'), 'colspan' => '4'));
  }

  return theme('table', $head, $rows, array('class' => 'openpgp keys')) . theme('pager', NULL, 20, 0);
}
