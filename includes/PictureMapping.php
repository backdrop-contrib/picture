<?php

class PictureMapping {

  /**
   * The picture mapping ID (machine name).
   *
   * @var string
   */
  public $machine_name;

  /**
   * The picture mapping label.
   *
   * @var string
   */
  public $label;

  /**
   * The picture mappings.
   *
   * @var array
   */
  public $mapping = array();

  /**
   * The breakpoint group.
   */
  public $breakpoint_group = '';

  /**
   * Constructor.
   * @see picture_mapping_object_factory().
   */
  public function __construct($schema, $data) {
    foreach ($schema['fields'] as $field => $info) {
      if (isset($data->{$field})) {
        $this->{$field} = empty($info['serialize']) ? $data->{$field} : unserialize($data->{$field});
      }
      else {
        $this->{$field} = NULL;
      }
    }

    if (isset($schema['join'])) {
      foreach ($schema['join'] as $join_key => $join) {
        $join_schema = ctools_export_get_schema($join['table']);
        if (!empty($join['load'])) {
          foreach ($join['load'] as $field) {
            $info = $join_schema['fields'][$field];
            $this->{$field} = empty($info['serialize']) ? $data->{$field} : unserialize($data->{$field});
          }
        }
      }
    }
    $this->loadBreakpointGroup();
    $this->loadAllMappings();
  }

  /**
   * Save the picture mapping.
   */
  public function save() {
    // Only save the keys, but return the full objects.
    $breakpoint_group = $this->getBreakpointGroup();
    if ($breakpoint_group && is_object($breakpoint_group)) {
      $this->setBreakpointGroup($breakpoint_group->machine_name);
    }
    $update = isset($this->id) ? array('id') : array();
    $return = drupal_write_record('picture_mapping', $this, $update);
    $this->loadBreakpointGroup();
    return $return;
  }

  /**
   * Create a duplicate.
   */
  public function createDuplicate() {
    $clone = clone $this;
    $clone->id = NULL;
    $clone->label = t('Clone of !label', array('!label' => check_plain($this->label)));
    $clone->mapping = $this->mapping;
    return $clone;
  }

  /**
   * Loads the breakpoint group.
   */
  protected function loadBreakpointGroup() {
    if ($this->breakpoint_group) {
      $breakpoint_group = breakpoints_breakpoint_group_load($this->breakpoint_group);
      $this->breakpoint_group = $breakpoint_group;
    }
  }

  /**
   * Loads all mappings and removes non-existing ones.
   */
  protected function loadAllMappings() {
    $loaded_mappings = $this->mapping;
    $all_mappings = array();
    if ($breakpoint_group = $this->breakpoint_group) {
      $breakpoints = $breakpoint_group->breakpoints;
      foreach ($breakpoints as $breakpoint_id) {
        $breakpoint = breakpoints_breakpoint_load_by_fullkey($breakpoint_id);
        // Get the mapping for the default multiplier.
        $all_mappings[$breakpoint_id]['1x'] = '';
        if (isset($loaded_mappings[$breakpoint->source_type][$breakpoint->source][$breakpoint->name]['1x'])) {
          $all_mappings[$breakpoint_id]['1x'] = $loaded_mappings[$breakpoint->source_type][$breakpoint->source][$breakpoint->name]['1x'];
        }

        // Get the mapping for the other multipliers.
        if (isset($breakpoint->multipliers) && !empty($breakpoint->multipliers)) {
          foreach ($breakpoint->multipliers as $multiplier => $status) {
            if ($status) {
              $all_mappings[$breakpoint_id][$multiplier] = '';
              if (isset($loaded_mappings[$breakpoint->source_type][$breakpoint->source][$breakpoint->name][$multiplier])) {
                $all_mappings[$breakpoint_id][$multiplier] = $loaded_mappings[$breakpoint->source_type][$breakpoint->source][$breakpoint->name][$multiplier];
              }
            }
          }
        }
      }
    }
    $this->mapping = $all_mappings;
  }

  /**
   * Check if there are mappings.
   */
  public function hasMappings() {
    $mapping_found = FALSE;
    foreach ($this->mapping as $multipliers) {
      foreach ($multipliers as $mapping_definition) {
        if (!$this::isEmptyMappingDefinition($mapping_definition)) {
          $mapping_found = TRUE;
          break 2;
        }
      }
    }
    return $mapping_found;
  }

  /**
   * {@inheritdoc}
   */
  public static function isEmptyMappingDefinition($mapping_definition) {
    if (!empty($mapping_definition)) {
      switch ($mapping_definition['mapping_type']) {
        case 'sizes':
          if ($mapping_definition['sizes'] && array_filter($mapping_definition['sizes_image_styles'])) {
            return FALSE;
          }
          break;
        case 'image_style':
          if ($mapping_definition['image_style']) {
            return FALSE;
          }
          break;
      }
    }
    return TRUE;
  }

  /**
   * Get the picture mappings.
   */
  public function getMappings() {
    return $this->mapping;
  }
}
