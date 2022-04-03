#!/bin/bash

grep -F myhash $1 | cut -f2- | sort -n > $(dirname $1)/$(basename $1 .log).hashes
