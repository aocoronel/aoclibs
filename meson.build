project('aoclibs', 'c',
  version: '0.1.0',
  default_options: ['buildtype=release', 'warning_level=3', 'c_std=c99', 'backend=ninja'],
  license: 'MIT'
)

# Libraries
# libprinth = shared_library('printh',
#   'info/printh.c',
#   install: true,
#   version: '1.0.0',
#   soversion: '1',
#   include_directories: include_directories('info')
# )

# libprintfc = shared_library('printfc',
#   'info/printfc.c',
#   install: true,
#   version: '1.0.0',
#   soversion: '1',
#   include_directories: include_directories('info')
# )

# libbashgen = shared_library('bashgen',
#   'automation/bashgen.c',
#   install: true,
#   version: '1.0.0',
#   soversion: '1',
#   include_directories: include_directories('automation')
# )
#
# libzshgen = shared_library('zshgen',
#   'automation/zshgen.c',
#   install: true,
#   version: '1.0.0',
#   soversion: '1',
#   include_directories: include_directories('automation')
# )

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
