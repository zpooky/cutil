cutil_test_src = files([
  'sp_test_bst.c',
  'sp_test_heap.c',
  'sp_test_heap_copy.c',
  'sp_test_huffman.c',
  'sp_test_perfect_hash_map.c',
  'sp_test_hash.c',
  'sp_test_hashset.c',
  'sp_test_sort.c',

  'sp_test_url.c',

  'test.c',
])

cutil_test = executable('thetest',
                      cutil_test_src,
                      include_directories: cutil_include,
                      link_args: cutil_link_args,
                      c_args: cutil_c_args,
                      # cpp_args: cutil_compile_options,
                      dependencies: [cutil_dep]
                     )

test('cutil test', cutil_test)
