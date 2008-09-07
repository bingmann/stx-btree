#!/bin/bash -x

build_flag() {
    local subdir="$1"
    local CXX="$2"
    local extraflags="$3"

    local stdflags='-DPACKAGE_NAME="stx-btree" -DPACKAGE_TARNAME="stx-btree" -DPACKAGE_VERSION="0.8.3" -DPACKAGE="stx-btree" -DVERSION="0.8.3" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDBOOL_H=1'

    stdflags="$stdflags -I. -I../include -g -W -Wall -Wold-style-cast -DNDEBUG"

    mkdir $subdir

    $CXX $stdflags $extraflags -o $subdir/speedtest speedtest.cc || exit

    strip -s $subdir/speedtest
}

build_flag "gcc-O0" "g++" "-O0"
build_flag "gcc-O1" "g++" "-O1"
build_flag "gcc-O2" "g++" "-O2"
build_flag "gcc-O2-ofp" "g++" "-O2 -fomit-frame-pointer"
build_flag "gcc-O3" "g++" "-O3"
build_flag "gcc-O3-ofp" "g++" "-O3 -fomit-frame-pointer"
build_flag "gcc-Os" "g++" "-Os"

build_flag "gcc-O3-i686-ofp" "g++" "-O3 -march=i686 -fomit-frame-pointer"

build_flag "icc-O0" "icc" "-O0"
build_flag "icc-O1" "icc" "-O1"
build_flag "icc-O2" "icc" "-O2"
build_flag "icc-O3" "icc" "-O3"
build_flag "icc-Os" "icc" "-Os"
