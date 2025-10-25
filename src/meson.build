cutil_src = [
  'sp_svec.c',
  'sp_vec.c',

  'sp_sink.c',
  'sp_fs.c',
  'sp_cbb.c',
  'sp_buf.c',
  'sp_xdg.c',

  'sp_stack.c',
  'sp_bst.c',
  'sp_queue.c',
  'sp_heap.c',

  'sp_uri.c',
  'sp_util.c',
  'sp_str.c',
  'sp_str_util.c',

  'sp_fnv.c',
  'sp_djb.c',

  'sp_huffman.c',
  'sp_perfect_hash_map.c',
  'sp_diffie_hellman.c',

  'sp_bitset.c',
  'sp_source.c',

  'sp_vec_copy.c',
  'sp_heap_copy.c',

  'sp_hashset.c',

  'sp_file_advisory_lock.c',

  'sp_callbacks.c',
  'sp_heap_array.c',
]
cutil_c_args = []
cutil_link_args = [
  '-fsanitize=address',
  '-fno-omit-frame-pointer',
]

cutil_include = include_directories('.')

cutil_lib = static_library('cutil',
              cutil_src,
              include_directories: cutil_include,
              # cpp_args: cutil_compile_options,
              link_args: cutil_link_args,
              c_args: cutil_c_args,
              )

cutil_dep = declare_dependency(
              include_directories: cutil_include,
              link_with: cutil_lib,
              )

subdir('test')
