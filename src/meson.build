cutil_src = [
  'sp_svec.c',
  'sp_vec.c',
  'sp_sink.c',
  'sp_faucet.c',
  'sp_fs.c',
  'sp_stack.c',
  'sp_bst.c',
  'sp_queue.c',
  'sp_uri.c',
  'sp_util.c',
  'sp_cbb.c',
  'sp_heap.c',
  'sp_buf.c',
  'sp_str.c',

  'sp_huffman.c',
  'sp_perfect_hash_map.c',
  'sp_diffie_hellman.c',
  'sp_hash.c',

  'sp_bitset.c',
  'sp_source.c',

]

cutil_link_options = [
  '-fsanitize=address',
  '-fno-omit-frame-pointer',
]

cutil_include = include_directories('.')

cutil_lib = static_library('cutil',
              cutil_src,
              include_directories: cutil_include,
              # cpp_args: cutil_compile_options,
              # dependencies: cutil_deps
              )

cutil_dep = declare_dependency(
              include_directories: cutil_include,
              link_with: cutil_lib,
              # dependencies: cutil_deps
              )

subdir('test')
