<?php

/**
 * @file
 * Layout functions and hooks for the D8 caching system backport.
 */

/**
 * Implements hook_preprocess_layout().
 */
function d8cache_preprocess_layout(&$variables) {
  $tags = array(
    'layout:' . $variables['layout']->name,
  );

  if (isset($variables['layout_info']['template']) && ($variables['layout_info']['template'] == 'layout--flexible')) {
    $tags[] = 'layout:flexible:' . $variables['layout']->layout_template;
  }

  backdrop_add_cache_tags($tags);
}

/**
 * Implements hook_layout_delete().
 */
function d8cache_layout_delete($layout) {
  _d8cache_invalidate_layout_cache_tags($layout);
}

/**
 * Implements hook_layout_disable().
 */
function d8cache_layout_disable($layout) {
  _d8cache_invalidate_layout_cache_tags($layout);
}

/**
 * Implements hook_layout_enable().
 */
function d8cache_layout_enable($layout) {
  _d8cache_invalidate_layout_cache_tags($layout);
}

/**
 * Implements hook_layout_insert().
 */
function d8cache_layout_insert($layout) {
  _d8cache_invalidate_layout_cache_tags($layout);
}

/**
 * Implements hook_layout_update().
 */
function d8cache_layout_update($layout) {
  _d8cache_invalidate_layout_cache_tags($layout);
}

/* -----------------------------------------------------------------------
 * Contrib hooks
 */

/* -----------------------------------------------------------------------
 * Public API
 */

/* -----------------------------------------------------------------------
 * Helper functions
 */


/**
 * Invalidates the cache tags for a given layout.
 *
 * @param Layout $layout
 *   The Layout to invalidate cache tags for.
 */
function _d8cache_invalidate_layout_cache_tags(Layout $layout) {
  $tags = array(
    'layout:' . $layout->name,
  );

  $info = layout_get_layout_template_info($layout->layout_template);
  if ($info['template'] === 'layout--flexible') {
    $tags[] = 'layout:flexible:' . $layout->layout_template;
  }

  backdrop_invalidate_cache_tags($tags);
}
