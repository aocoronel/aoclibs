alias c := clean
alias b := build
alias bf := buildf
alias r := reconfigure
alias i := install
alias s := setup

# === Misc ===

# List (default)
[group: 'misc']
list:
  just --list

# Clean build files
[group: 'misc']
clean:
  rm -rf build

# === Release ===

# Install project
[group: 'release']
install:
  just setup release
  meson install -C build

# === Development ===

# Reconfigure the build environment (debug, debugoptimized, release)
[group: 'dev']
reconfigure +buildtype='debugoptimized':
  meson setup --buildtype '{{ buildtype }}' build --reconfigure

# Configure the build environment (debug, debugoptimized, release)
[group: 'dev']
setup +buildtype='debugoptimized':
  meson setup --buildtype '{{ buildtype }}' build

# Build project
[group: 'dev']
build: setup
  meson compile -C build

# Reconfigure and build project
[group: 'dev']
buildf: reconfigure
  meson compile -C build

# === Testing ===

[group: 'test']
[working-directory: 'tests']
test:
  export ASAN_OPTIONS=halt_on_error=1:abort_on_error=1:print_summary=1
  export UBSAN_OPTIONS=halt_on_error=1:abort_on_error=1:print_summary=1:print_stacktrace=1
  export MSAN_OPTIONS=halt_on_error=1:abort_on_error=1:print_summary=1:print_stacktrace=1
  export MALLOC_PERTURB_=$((RANDOM % 255 + 1))
  make
  ./test
