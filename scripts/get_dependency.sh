#!/usr/bin/env sh

TUNIT_URL=https://codeberg.org/aocoronel/tunit
TUNIT_DIR=tunit

[ ! -d "$TUNIT_DIR" ] && git clone "$TUNIT_URL" "$TUNIT_DIR"

exit 0
