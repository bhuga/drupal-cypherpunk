<?php

/**
 * Implementation of hook_profile_details().
 *
 * Returns a description of the profile for the initial installation screen.
 */
function cypherpunk_profile_details() {
  return array(
    'name'        => 'Cypherpunk',
    'description' => 'Select this profile to install the Cypherpunk Drupal platform.',
  );
}

/**
 * Implementation of hook_profile_modules().
 *
 * Returns an array of the modules to be enabled when this profile is
 * installed.
 */
function cypherpunk_profile_modules() {
  return array(
    // Required core modules
    'system', 'filter', 'block', 'user', 'node',

    // Optional core modules
    'comment', 'taxonomy', 'path', 'menu', 'trigger', 'dblog', 'help', 'color',

    // Cypherpunk modules
    // TODO
  );
}

/**
 * Implementation of hook_profile_task_list().
 *
 * Returns a list of tasks that this profile supports.
 */
function cypherpunk_profile_task_list() {}

/**
 * Implementation of hook_profile_tasks().
 *
 * Performs any final installation tasks for this profile.
 */
function cypherpunk_profile_tasks(&$task, $url) {
  // Insert some default user-defined node types into the database:
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Page'),
      'module' => 'node',
      'description' => st("A <em>page</em>, similar in form to a <em>story</em>, is a simple method for creating and displaying information that rarely changes, such as an \"About us\" section of a website. By default, a <em>page</em> entry does not allow visitor comments and is not featured on the site's initial home page."),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
      'help' => '',
      'min_word_count' => '',
    ),
    array(
      'type' => 'story',
      'name' => st('Story'),
      'module' => 'node',
      'description' => st("A <em>story</em>, similar in form to a <em>page</em>, is ideal for creating and displaying content that informs or engages website visitors. Press releases, site announcements, and informal blog-like entries may all be created with a <em>story</em> entry. By default, a <em>story</em> entry is automatically featured on the site's initial home page, and provides the ability to post comments."),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
      'help' => '',
      'min_word_count' => '',
    ),
  );

  foreach ($types as $type) {
    node_type_save($type = (object)_node_type_set_defaults($type));
  }

  // Default page to not be promoted and have comments disabled:
  variable_set('node_options_page', array('status'));
  variable_set('comment_page', COMMENT_NODE_DISABLED);

  // Don't display date and author information for page nodes by default:
  $theme_settings = variable_get('theme_settings', array());
  $theme_settings['toggle_node_info_page'] = FALSE;
  variable_set('theme_settings', $theme_settings);

  // Update the menu router information:
  menu_rebuild();
}

/**
 * Implementation of hook_form_alter().
 *
 * Allows the profile to alter the site-configuration form. This is called
 * through custom invocation, so $form_state is not populated.
 */
function cypherpunk_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'install_configure') {
    // Set a default for the site name field:
    $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
  }
}
