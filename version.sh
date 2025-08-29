#!/usr/bin/env bash
#  SPDX-FileCopyrightText: © 2025 Remo Dentato (rdentato@gmail.com)
#  SPDX-License-Identifier: MIT

# Simple script to propagate the version in each file that stores it.

# Set the major, minor and patch numbers according to the "Semantic Versioning" rules (https://semver.org)
VER_MAJOR=0
VER_MINOR=3
VER_PATCH=0

VER_STAGE=C
# The development stage (and any additional version info) can be one of:
# A -> alpha
# B -> beta
# C -> RC (release candidate)
# F -> Final

## -- SET ONCE FOR YOUR LIBRARY
VER_LIB=fsm
VER_MACRO=FSM_VERSION

## -- DO NOT TOUCH ANYTHING BELOW THIS LINE

VER_HDR_H=`printf "0x%02d%02d%03d%c" ${VER_MAJOR} ${VER_MINOR} ${VER_PATCH} ${VER_STAGE}`

case "$VER_STAGE" in
  C) STAGE="-RC"    ;;
  A) STAGE="-alpha" ;;
  B) STAGE="-beta"  ;;
  F) STAGE=""      ;;
  *) STAGE="-$VER_STAGE" ;;
esac

VER_SPDX=`printf "%d.%d.%03d" ${VER_MAJOR} ${VER_MINOR} ${VER_PATCH}`
VER_SPDX=${VER_SPDX}${STAGE}
echo ${VER_SPDX}

VER_README="${VER_SPDX// /%20}"
VER_README="${VER_SPDX//-/%20}"

VER_HDR_FULL="#define ${VER_MACRO} ${VER_HDR_H}"
VER_SPDX_FULL="PackageVersion: ${VER_SPDX}"
VER_README_FULL="[![Version](https:\/\/img.shields.io\/badge\/version-${VER_README}-blue.svg)](https:\/\/github.com\/rdentato\/${VER_LIB})"

VER_DATE=`date -u '+%Y-%m-%dT%H:%M:%SZ'`

sed -i -e "s/^#define ${VER_MACRO} .*/${VER_HDR_FULL}/" src/${VER_LIB}.h
sed -i -e "s/^PackageVersion: .*/${VER_SPDX_FULL}/" ${VER_LIB}.spdx
sed -i -e "s/^Created: .*/Created: ${VER_DATE}/" ${VER_LIB}.spdx
sed -i -e "s/^\[!\[Version\].*/${VER_README_FULL}/" README.md

echo "${VER_LIB} version set to: ${VER_SPDX} on ${VER_DATE}"
