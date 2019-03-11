#!/bin/bash

print () {
  printf "%s\n" "$1";
}


FLOW_FILE="${1}";
ANIMATION_FILE="${2}";
SCRIPT_DIR=`dirname "${0}"`;
FRAME_GEN=`printf "%s/generic_executable" "${SCRIPT_DIR}"`;

## Check if executable exists
if test ! -f "${FRAME_GEN}"; then
  print "Cannot find frame generator at expected location \"${FRAME_GEN}\"";
  exit `false`;
fi

## Check if specified input file exists
if test ! -f "${FLOW_FILE}"; then
  print "Input file \"$1\" does not exist!";
  exit `false`;
fi


## Generate PPM frame images
print "Generating visualization frames...";
"${FRAME_GEN}" "${FLOW_FILE}";

## Convert frames to GIFs
print "Converting frames to GIFs...";
ls frame-*ppm | while read f; do
  b=`basename $f .ppm`;
  print "$f --> ${b}.gif";
  convert "${f}" "${b}.gif";
done

## Merge frame GIFs
print "Merging frames into single animated GIF...";
gifsicle --loop --delay 5  --merge frame-*gif --output "${ANIMATION_FILE}";


exit `:`;

