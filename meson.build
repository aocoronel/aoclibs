project('aoclibs', 'c',
  version: '0.1.0',
  default_options: ['buildtype=release', 'warning_level=3', 'c_std=c99', 'backend=ninja'],
  license: 'MIT'
)

add_project_arguments('-DDEBUG_HEAP', '-DTUNIT', language: 'c')

tunit = static_library('tunit',
  'src/tunit.c',
  install: false,
  include_directories: include_directories('.')
)

liberror = static_library('error',
  'src/error.c',
  'src/assert.c',
  'src/printfc.c',
  'src/ok.c',
  'src/werr.c',
  'src/read_err.c',
  'src/panic.c',
  install: false,
  # version: '0.1.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

libcli = static_library('cli',
  'src/cli/bashgen.c',
  'src/cli/zshgen.c',
  'src/cli/printh.c',
  'src/cli/normalize_shell_name.c',
  install: false,
  # version: '1.0.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

libmem_source = [
  'src/mem/basename.c',
  'src/mem/dirname.c',
  'src/mem/strcat.c',
  'src/mem/strcpy.c',
  'src/mem/streq.c',
  'src/mem/strfind.c',
  'src/mem/strlen.c',
  'src/mem/strtob.c',
  'src/mem/strtod.c',
  'src/mem/strtof.c',
  'src/mem/strtol.c',
]

libmem = static_library('mem',
  libmem_source,
  install: false,
  # version: '1.0.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

libio = static_library('io',
  'src/io/get_spinner.c',
  'src/io/print_indent.c',
  install: false,
  # version: '1.0.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

libfs = static_library('fs',
  'src/fs/file/check_ft.c',
  'src/fs/dir/walker.c',
  install: false,
  # version: '1.0.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

libdebug = static_library('debug',
  'src/debug/heap.c',
  install: false,
  # version: '1.0.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

libheap = static_library('heap',
  'src/heap/calloc.c',
  'src/heap/malloc.c',
  'src/heap/realloc.c',
  'src/heap/free.c',
  install: false,
  # version: '1.0.0',
  # soversion: '1',
  include_directories: include_directories('.')
)

static_library('aoclibs',
  objects :
    [
      libio.extract_all_objects(recursive:true),
      libcli.extract_all_objects(recursive:true),
      libfs.extract_all_objects(recursive:true),
      libheap.extract_all_objects(recursive:true),
      liberror.extract_all_objects(recursive:true),
      libmem.extract_all_objects(recursive:true)
    ]
)

# info_headers_cmd = run_command(
#   'find', 'info', '-type', 'f', '-name', '*.h',
#   check: true,
# )
# info_headers = files(info_headers_cmd.stdout().strip().split('\n'))

# automation_headers_cmd = run_command(
#   'find', 'automation', '-type', 'f', '-name', '*.h',
#   check: true,
# )
# automation_headers = files(automation_headers_cmd.stdout().strip().split('\n'))

# install_headers(
#   info_headers,
#   automation_headers,
#   subdir: 'aoclibs'
# )
