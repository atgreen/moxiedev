#!/bin/sh

# run this in the gcc source directory to fix things before running svn.
# mercurial seems to ignore empty dirs that svn needs.

for d in `find ./ -name \*.svn`; do
  for e in tmp/prop-base tmp/props tmp/text-base; do
    if ! test -d $d/$e; then
      mkdir -p $d/$e
    fi
  done
done
