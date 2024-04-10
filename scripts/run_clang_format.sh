#!/bin/bash

DISTRO=''

if [ -f "/etc/debian_version" ]; then
  DISTRO='debian'
fi

if [ -x "$(command -v clang-format-16)" ]; then
  find src -iname '*.h' -o -iname '*.c' -o -iname '*.cpp' | xargs clang-format-16 -i
else
  echo "clang-format-16 is not installed"
  case $DISTRO in
    debian)
      echo "Try 'sudo apt install clang-format-16'"
      ;;
  esac
fi
