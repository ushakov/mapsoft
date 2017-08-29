#### Cache and SizeCache classes -- cache of objects of type V, keyed by type K

This code have been written by Max Ushakov. There are two caches with
different eviction policies:

SizeCache: eviction starts whenever number of elements exceeds some limit
(cache size) set at construction and removed least recently used elements.

SizeCache: eviction starts whenever total stored size exceeds threshold
set at construction and removed least recently used elements. To enable
that, V must have method size() returning the size of object.
