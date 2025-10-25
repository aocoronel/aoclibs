# List (default)
list:
  just --list

# Build
build:
  cmake -B build .
  cmake --build build

# Install
install:
  cmake --install build

# Clean files
clean:
  rm -rf build
