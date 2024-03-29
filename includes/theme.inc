<?php

/**
 * @file
 * Theme functions and hooks for the D8 caching system backport.
 */

/**
 * Implements hook_preprocess_username().
 */
function d8cache_preprocess_username(&$variables) {
  if ($variables['uid'] > 0) {
    backdrop_add_cache_tags(array('user:' . $variables['uid']));
  }
}

/**
 * Implements hook_preprocess_search_results().
 */
function d8cache_preprocess_search_results(&$variables) {
  $tags = array();
  $tags[] = 'search_index';

  if (!empty($variables['module'])) {
    $tags[] = 'search_index:' . $variables['module'];
  }

  backdrop_add_cache_tags($tags);
}
