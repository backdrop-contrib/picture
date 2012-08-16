<?php

/**
 * @file
 * Picture - map breakpoints and image styles
 */

/**
 * Admin form
 */
function picture_admin_breakpoints($form, &$form_state, $breakpoint_group_name = '') {
  // Show a list of all groups if no group name is given.
  if ($breakpoint_group_name == '' || $breakpoint_group_name == 'global') {
    return picture_admin_breakpoints_overview_page();
  }
  $machine_name = 'picture.mappings.' . $breakpoint_group_name;
  $form = array();

  $mappings = picture_mapping_load($breakpoint_group_name);
  $mappings = $mappings ? $mappings : new stdclass();

  $form['picture_mapping'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
  );
  $form['picture_mapping']['machine_name'] = array(
    '#type' => 'value',
    '#value' => isset($mappings->machine_name) ? $mappings->machine_name : $machine_name,
  );
  $form['picture_mapping']['breakpoint_group'] = array(
    '#type' => 'value',
    '#value' => isset($mappings->breakpoint_group) ? $mappings->breakpoint_group : $breakpoint_group_name,
  );
  if (isset($mappings->id)) {
    $form['picture_mapping']['id'] = array(
      '#type' => 'value',
      '#value' => $mappings->id,
    );
  }

  $breakpoints = array();
  $breakpoint_group = breakpoints_breakpoint_group_load($breakpoint_group_name);
  $weight = 0;
  foreach ($breakpoint_group->breakpoints as $breakpoint_name) {
    $breakpoint = breakpoints_breakpoint_load_by_fullkey($breakpoint_name);
    if ($breakpoint && $breakpoint->status) {
      $breakpoint->global_weight = $breakpoint->weight;
      $breakpoint->weight = $weight++;
      $breakpoints[$breakpoint_name] = $breakpoint;
    }
  }

  $image_styles = image_style_options(TRUE);
  foreach ($breakpoints as $breakpoint_name => $breakpoint) {
    $label = '1x ' . $breakpoint->name . ' [' . $breakpoint->breakpoint . ']';
    $form['picture_mapping']['mapping'][$breakpoint_name]['1x'] = array(
      '#title' => check_plain($label),
      '#type' => 'select',
      '#options' => $image_styles,
      '#default_value' => isset($mappings->mapping[$breakpoint_name]['1x']) ? $mappings->mapping[$breakpoint_name]['1x'] : '',
    );
    if (isset($breakpoint->multipliers) && !empty($breakpoint->multipliers)) {
      foreach ($breakpoint->multipliers as $multiplier => $status) {
        if ($status) {
          $label = $multiplier . ' ' . $breakpoint->name . ' [' . $breakpoint->breakpoint . ']';
          $form['picture_mapping']['mapping'][$breakpoint_name][$multiplier] = array(
            '#title' => check_plain($label),
            '#type' => 'select',
            '#options' => $image_styles,
            '#default_value' => isset($mappings->mapping[$breakpoint_name][$multiplier]) ? $mappings->mapping[$breakpoint_name][$multiplier] : '',
          );
        }
      }
    }
  }

  // Buttons
  $form['buttons'] = array(
    '#type' => 'container',
  );

  // Submit button
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Admin form overview page.
 */
function picture_admin_breakpoints_overview_page() {
  $links = array();
  $breakpoint_groups = breakpoints_breakpoint_group_load_all();
  foreach ($breakpoint_groups as $breakpoint_group) {
    $links[] = l($breakpoint_group->name, 'admin/config/media/picture/groups/' . $breakpoint_group->machine_name);
  }
  return array(
    '#type' => 'container',
    '#theme' => 'item_list',
    '#items' => $links,
  );
}

/**
 * Admin form submit.
 */
function picture_admin_breakpoints_submit($form, &$form_state) {
  $mapping = (object)$form_state['values']['picture_mapping'];
  picture_mapping_save($mapping);
}
