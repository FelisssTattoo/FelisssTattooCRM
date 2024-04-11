#!/bin/bash

DISTRO=''

if [ -f "/etc/debian_version" ]; then
  DISTRO='debian'
fi

if [ -x "$(command -v clang-tidy-16)" ]; then
  clang-tidy-16 -p build src/*
else
  echo "clang-tidy-16 is not installed"
  case $DISTRO in
    debian)
      echo "Try 'sudo apt install clang-tidy-16'"
      ;;
  esac
fi