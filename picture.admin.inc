<?php
/**
 * @file
 * Picture - map breakpoints and image styles.
 */

/**
 * Select breakpoint group form.
 */
function picture_select_breakpoint_group_form($form, &$form_state) {
  $groups = breakpoints_breakpoint_group_load_all();
  $options = array();
  foreach ($groups as $group) {
    $options[$group->machine_name] = $group->name;
  }
  if ($options) {
    $form['breakpoint_group'] = array(
      '#title' => t('Breakpoint group'),
      '#type' => 'select',
      '#required' => TRUE,
      '#description' => t('Select a breakpoint group.'),
      '#options' => $options,
      '#default_value' => $form_state['item']->getBreakpointGroup() ? $form_state['item']->getBreakpointGroup()->machine_name : NULL,
    );
  }
  else {
    $form['breakpoint_group'] = array(
      '#title' => t('Breakpoint group'),
      '#type' => 'item',
      '#markup' => t(
        'There are no breakpoint groups. !create_link.',
        array('!create_link' => l(t('Create a breakpoint group'), 'admin/config/media/breakpoints/groups/add'))
      ),
    );
  }

  return $form;
}

/**
 * Validate breakpoint group.
 */
function picture_select_breakpoint_group_form_validate($form, &$form_state) {
  if (!isset($form_state['values']['breakpoint_group']) || empty($form_state['values']['breakpoint_group'])) {
    form_error($form['breakpoint_group'], t('No breakpoint group was selected.'));
  }
}

/**
 * Submit callback.
 */
function picture_select_breakpoint_group_form_submit($form, &$form_state) {
  $form_state['item']->setBreakpointGroup($form_state['values']['breakpoint_group']);
}

/**
 * Validate callback.
 */
function picture_mapping_form_validate($form, &$form_state) {
  $picture_mapping = $form_state['item'];
  if ($form_state['triggering_element']['#type'] != 'submit') {
    $picture_mapping->setBreakpointGroup($form_state['values']['breakpoint_group']);
    $form_state['rebuild'] = TRUE;
    return;
  }
  $picture_mapping->setLabel($form_state['values']['label']);
  $picture_mapping->setMachineName($form_state['values']['machine_name']);
  $picture_mapping->setMappings($form_state['values']['mapping']);
  // Make sure at least one mapping is defined.
  if (!$picture_mapping->hasMappings()) {
    form_error($form['mapping'], t('Please select at least one mapping.'));
  }
}

/**
 * Chooses which picture groups are available in the CKEditor image dialog.
 */
function picture_admin_settings() {
  $config = config('picture.settings');
  $form = array();
  $picture_mappings = picture_get_mapping_options();

  // Check if picture mappings have been configured before proceeding.
  if ($picture_mappings) {
    // Create a settings form.
    $form['description'] = array(
      '#type' => 'item',
      '#title' => t('Choose which picture mappings will be available in the CKEditor image dialog.'),
      '#description' => t('See picture_wysiwyg.css for an example of how to style these images in your theme using the selectors suggested below.'),
    );

    // Retrieve pre-existing settings.
    $ckeditor_mappings = $config->get('picture_ckeditor_mappings');

    // Make option list for CKEditor default mapping.
    $ckeditor_default_mapping_options = array('not_set' => 'Not Set');

    // Loop through each picture mapping and place a checkbox and weight.
    foreach ($picture_mappings as $machine_name => $display_name) {
      // Add option to CKEditor default mapping if enabled.
      if (isset($ckeditor_mappings[$machine_name]) && $ckeditor_mappings[$machine_name]['enabled']) {
        $ckeditor_default_mapping_options[$machine_name] = $display_name;
      }

      $form[$machine_name] = array(
        '#type' => 'fieldset',
        '#title' => t('@name picture mapping', array('@name' => $display_name)),
      );
      $form[$machine_name]['enabled'] = array(
        '#type' => 'checkbox',
        '#default_value' => isset($ckeditor_mappings[$machine_name]) ? $ckeditor_mappings[$machine_name]['enabled'] : 0,
        '#title' => t('Include @name picture mapping in the CKEditor image dialog', array('@name' => $display_name)),
      );

      $form[$machine_name]['css'] = array(
        '#type' => 'item',
        '#markup' => t('Consider using the selector <code>span[data-picture-mapping="@machine_name"]</code> in your theme CSS.', array(
          '@machine_name' => $machine_name,
        )),
      );
      $form[$machine_name]['weight'] = array(
        '#type' => 'select',
        '#title' => t('Weight'),
        '#options' => backdrop_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
        '#default_value' => isset($ckeditor_mappings[$machine_name]) ? $ckeditor_mappings[$machine_name]['weight'] : 1,
        '#description' => t('Control the sort order of picture mappings in the CKEditor "size" drop-down. Higher weights sink to the bottom of the list.'),
      );
      $form[$machine_name]['fallback'] = array(
        '#type' => 'select',
        '#title' => t('Fallback image style'),
        '#options' => backdrop_map_assoc(array_keys(image_styles())) + array(
          PICTURE_EMPTY_IMAGE => t('Empty image'),
        ),
        '#default_value' => isset($ckeditor_mappings[$machine_name]) ? $ckeditor_mappings[$machine_name]['fallback'] : NULL,
      );
      $form[$machine_name]['lazyload'] = array(
        '#title' => t('Picture lazyload'),
        '#description' => t('Image will be rendered when it appears in viewport, helps to optimize page load speed.'),
        '#type' => 'checkbox',
        '#default_value' => isset($ckeditor_mappings[$machine_name]['lazyload']) ? $ckeditor_mappings[$machine_name]['lazyload'] : 0,
      );

      $form[$machine_name]['lazyload_aspect_ratio'] = array(
        '#title' => t('Keep aspect ratio'),
        '#type' => 'checkbox',
        '#description' => t('Preserve the space for the image being lazyloaded to avoid layout reflows. <br /> Image ratio is defined per breakpoint, make sure all images from srcset have the same ratio. <br />Output example: !example',
          array('!example' => htmlentities('<source media="(...)" data-srcset="image_400x200.jpg x1, image_800x400.jpg x2, image_1200x600.jpg x3" data-aspectratio="2" />'))
        ),
        '#default_value' => isset($ckeditor_mappings[$machine_name]['lazyload_aspect_ratio']) ? $ckeditor_mappings[$machine_name]['lazyload_aspect_ratio'] : 0,
        '#states' => array(
          'visible' => array(
            ':input[name="article_responsive_image[lazyload]"]' => array('checked' => TRUE),
            ':input[name="article_responsive_image[fallback]"]' => array('value' => PICTURE_EMPTY_IMAGE),
          ),
        ),
      );
    }
    $form['#tree'] = TRUE;
    $form['ckeditor_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label in the CKEditor image dialog'),
      '#description' => t('This sets the label for the drop-down select box containing these picture mappings in the CKEditor image dialog'),
      '#default_value' => $config->get('picture_ckeditor_label'),
    );
    $form['ckeditor_default_mapping'] = array(
      '#type' => 'select',
      '#title' => t('Default CKEditor Picture Mapping'),
      '#options' => $ckeditor_default_mapping_options,
      '#default_value' => $config->get('picture_ckeditor_default_mapping'),
      '#description' => t('This sets the default picture mapping in CKEditor'),
    );
  }
  else {
    $form['info'] = array(
      '#markup' => t('No picture mappings found. !create', array('!create' => l(t('Create one now.'), 'admin/config/media/picture/add'))),
    );
  }

  $form['picture_js_scope'] = array(
    '#title' => t('Picture JavaScript scope'),
    '#description' => t("For performance reasons, JS should be loaded in the footer. However, you can move the Picture module's JS to the header so that it loads before JS from other modules like Views Slideshow."),
    '#type' => 'radios',
    '#options' => array(
      'header' => t('Header'),
      'footer' => t('Footer'),
    ),
    '#default_value' => $config->get('picture_js_scope'),
  );

  $form['picture_polyfill_version'] = array(
    '#title' => t('Picture polyfill to use'),
    '#description' => t('For performance reasons, use the minified version.'),
    '#type' => 'radios',
    '#options' => array(
      'min' => t('Minified polyfill'),
      'dev' => t('Development polyfill'),
      'own' => t('Use custom polyfill'),
    ),
    '#default_value' => $config->get('picture_polyfill_version'),
  );

  $form['picture_fallback_method'] = array(
    '#title' => t('Picture fallback method'),
    '#description' => t('Use the fallback method, the default is to use srcset since it avoids double downloads, but src is the right method and will also work if the browser does not support javascript.'),
    '#type' => 'radios',
    '#options' => array(
      'src' => t('src fallback'),
      'srcset' => t('srcset fallback'),
    ),
    '#default_value' => $config->get('picture_fallback_method'),
  );

  $form['picture_img_sizes_output_method'] = array(
    '#title' => t('"Image with sizes" output method'),
    '#description' => t('"srcset on Image tag" creates the correct markup, however "picture element" will prevent duplicate downloads.'),
    '#type' => 'radios',
    '#options' => array(
      'img_srcset' => t('srcset on Image tag'),
      'picture_element' => t('Picture element'),
    ),
    '#default_value' => $config->get('picture_img_sizes_output_method'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  return $form;
}

/**
 * Validate handler for the picture_ckeditor_settings form.
 *
 * It checks that a fallback image style is selected for every
 * picture mapping that has been enabled for the CKEditor image dialog.
 */
function picture_admin_settings_validate($form, &$form_state) {
  $picture_mappings = picture_mapping_load_all();
  if ($picture_mappings) {
    foreach ($picture_mappings as $picture_mapping) {
      $machine_name = $picture_mapping->getMachineName();
      if ($form_state['values'][$machine_name]['enabled'] == 1) {
        if (empty($form_state['values'][$machine_name]['fallback'])) {
          form_set_error($machine_name . '][fallback', t('Please choose a fallback image style for this picture mapping'));
        }
      }
    }
  }
}

/**
 * Submit handler, places chosen picture groups into the variables table.
 */
function picture_admin_settings_submit($form, &$form_state) {
  $picture_mappings = picture_mapping_load_all();
  $ckeditor_mappings = array();
  if ($picture_mappings) {
    // Loop each picture mapping and record the settings.
    foreach ($picture_mappings as $picture_mapping) {
      $machine_name = $picture_mapping->getMachineName();
      $ckeditor_mappings[$machine_name]['enabled'] = $form_state['values'][$machine_name]['enabled'];
      $ckeditor_mappings[$machine_name]['weight'] = $form_state['values'][$machine_name]['weight'];
      $ckeditor_mappings[$machine_name]['fallback'] = $form_state['values'][$machine_name]['fallback'];
      $ckeditor_mappings[$machine_name]['lazyload'] = $form_state['values'][$machine_name]['lazyload'];
      $ckeditor_mappings[$machine_name]['lazyload_aspect_ratio'] = $form_state['values'][$machine_name]['lazyload_aspect_ratio'];
    }

    uasort($ckeditor_mappings, 'picture_compare_weights');
    config_set('picture.settings', 'picture_ckeditor_mappings', $ckeditor_mappings);
    config_set('picture.settings', 'picture_ckeditor_label', $form_state['values']['ckeditor_label']);
    config_set('picture.settings', 'picture_ckeditor_default_mapping', $form_state['values']['ckeditor_default_mapping']);
  }

  config_set('picture.settings', 'picture_js_scope', $form_state['values']['picture_js_scope']);
  config_set('picture.settings', 'picture_polyfill_version', $form_state['values']['picture_polyfill_version']);
  config_set('picture.settings', 'picture_fallback_method', $form_state['values']['picture_fallback_method']);
  config_set('picture.settings', 'picture_img_sizes_output_method', $form_state['values']['picture_img_sizes_output_method']);
  backdrop_set_message(t('Your settings have been saved'));
}

/**
 * Sort picture groups for the CKEditor image dialog.
 */
function picture_compare_weights($a, $b) {
  if ($a['weight'] == $b['weight']) {
    return 0;
  }
  return ($a['weight'] < $b['weight']) ? -1 : 1;
}
