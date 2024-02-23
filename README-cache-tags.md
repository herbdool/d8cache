# Cache tags

## Why?

Cache tags provide a declarative way to track which cache items depend on some data managed by Drupal.

This is essential for a content management system/framework like Drupal because the same content can be reused in many ways. In other words: it is impossible to know ahead of time where some content is going to be used. In any of the places where the content is used, it may be cached. Which means the same content could be cached in dozens of places. Which then brings us to the famous quote "There are only two hard things in Computer Science: cache invalidation and naming things." — that is, how are you going to invalidate all cache items where the content is being used?

Note: Drupal 7 offered 3 ways of invalidating cache items: invalidate a specific CID, invalidate using a CID prefix, or invalidate everything in a cache bin. None of those 3 methods allow us to invalidate the cache items that contain an entity that was modified, because that was impossible to know!

## What?

A cache tag is a string.

Cache tags are passed around in sets (order doesn't matter) of strings, so they are typehinted to string[]. They're sets because a single cache item can depend on (be invalidated by) many cache tags.

## Syntax

By convention, they are of the form thing:identifier — and when there's no concept of multiple instances of a thing, it is of the form thing. The only rule is that it cannot contain spaces.

There is no strict syntax.

Examples:

node:5 — cache tag for Node entity 5 (invalidated whenever it changes)
user:3 — cache tag for User entity 3 (invalidated whenever it changes)
node_list — list cache tag for Node entities (invalidated whenever any Node entity is updated, deleted or created, i.e., when a listing of nodes may need to change). Applicable to any entity type in following format: {entity_type}_list.
node_list:article — list cache tag for the article bundle (content type). Applicable to any entity + bundle type in following format: {entity_type}_list:{bundle}.
config:node_type_list — list cache tag for Node type entities (invalidated whenever any content types are updated, deleted or created). Applicable to any entity type in the following format: config:{entity_bundle_type}_list.
config:system.performance — cache tag for the system.performance configuration
library_info — cache tag for asset libraries

## Common cache tags

The data that Drupal manages fall in 3 categories:

entities — these have cache tags of the form <entity type ID>:<entity ID> as well as <entity type ID>_list and <entity type ID>_list:<bundle> to invalidate lists of entities. Config entity types use the cache tag of the underlying configuration object.
configuration — these have cache tags of the form config:<configuration name>
custom (for example library_info)
Drupal provides cache tags for entities & configuration automatically — see the Entity base class and the ConfigBase base class. (All specific entity types and configuration objects inherit from those.)

Although many entity types follow a predictable cache tag format of <entity type ID>:<entity ID>, third-party code shouldn't rely on this. Instead, it should retrieve cache tags to invalidate for a single entity using its::getCacheTags() method, e.g., $node->getCacheTags(), $user->getCacheTags(), $view->getCacheTags() etc.

In addition, it may be necessary to invalidate listings-based caches that depend on data from the entity in question (e.g., refreshing the rendered HTML for a listing when a new entity for it is created): this can be done using EntityTypeInterface::getListCacheTags(), then invalidating any returned by that method along with the entity's own tag(s). Starting with Drupal 8.9 (change notice), entities with bundles also automatically have a more specific cache tag that includes their bundle, to allow for more targetted invalidation of lists.

It is also possible to define custom, more specific cache tags based on values that entities have, for example a term reference field for lists that show entities that have a certain term. Invalidation for such tags can be put in custom presave/delete entity hooks:

```php
function yourmodule_node_presave(NodeInterface $node) {
  $tags = [];
  if ($node->hasField('field_category')) {
    foreach ($node->get('field_category') as $item) {
      $tags[] = 'mysite:node:category:' . $item->target_id;
    }
  }
  if ($tags) {
    Cache::invalidateTags($tags);
  }
}
```

These tags can then be used in code and in views using the contributed module Views Custom Cache Tag.

Note: There is currently no API to get per-bundle and more specific cache tags from an entity or other object. That is because it is not the entity that decided which list cache tags are relevant for a certain list/query, that depends on the query itself. Future Drupal core versions will likely improve out of the box support for per-bundle cache tags and for example integrate them into the entity query builder and views.


## How?

### Setting

Any cache backend should implement CacheBackendInterface, so when you set a cache item with the ::set() method, provide third and fourth arguments e.g:

```php
$cache_backend->set(
  $cid, $data, Cache::PERMANENT, ['node:5', 'user:7']
);
```

This stores a cache item with ID $cid permanently (i.e., stored indefinitely), but makes it susceptible to invalidation through either the node:5 or user:7 cache tags.

### Invalidating

Tagged cache items are invalidated via their tags, using cache_tags.invalidator:invalidateTags() (or, when you cannot inject the cache_tags.invalidator service: Cache::invalidateTags()), which accepts a set of cache tags (string[]).

Note: this invalidates items tagged with given tags, across all cache bins. This is because it doesn't make sense to invalidate cache tags on individual bins, because the data that has been modified, whose cache tags are being invalidated, can have dependencies on cache items in other cache bins.

## Debugging

All of the above is helpful information when debugging something that is being cached. But, there's one more thing: let's say something is being cached with the cache tags ['foo', 'bar']. Then the corresponding cache item will have a tags column (assuming the database cache back-end for a moment) with the following value:

`bar foo`

In other words:

cache tags are separated by space
cache tags are sorted alphabetically
That should make it much easier to analyze & debug caches!

### Headers (debugging)

Finally: it is easy to see which cache tags a certain response depends on (and thus is invalidated by): one must only look at the X-Drupal-Cache-Tags header!

(This is also why spaces are forbidden: because the X-Drupal-Cache-Tags header, just like many HTTP headers, uses spaces to separate values.)

