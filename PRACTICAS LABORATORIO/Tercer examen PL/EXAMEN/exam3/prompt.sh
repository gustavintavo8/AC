#!/bin/bash

function Usage {
  echo "Usage: prompt.sh <uo>" >&2
}

if [ "$#" -ne 1 ]; then
   Usage
else
   PS1="\t|"$1"@\w$ "
fi
