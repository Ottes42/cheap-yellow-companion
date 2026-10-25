#pragma once
// Memory is no bottleneck in the browser: a large block is always free
#include <cstddef>
#define MALLOC_CAP_8BIT 0
inline size_t heap_caps_get_largest_free_block(int) { return 16 * 1024 * 1024; }
inline size_t heap_caps_get_free_size(int) { return 16 * 1024 * 1024; }
