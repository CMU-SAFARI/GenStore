#!/bin/bash

grep -F myhash $1 | cut -f2- | sort -n | awk '{printf("%s\t",$0); for (i=0;i<'$2';i++) { printf("A"); }; printf("\n")}' > $1.hashes
