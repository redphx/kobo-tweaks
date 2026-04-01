#!/bin/bash

readonly INPUT_FILE="$1"
readonly FILENAME=$(basename -- "$INPUT_FILE")
readonly BASE="${FILENAME%.*}"
readonly SIZES=("57x54" "64x61" "102x97" "116x110" "133x126")

if [[ ! -f "$INPUT_FILE" ]]; then
    printf "Error: File %s not found.\n" "$INPUT_FILE" >&2
    exit 1
fi

for dim in "${SIZES[@]}"; do
    magick "$INPUT_FILE" -background transparent -density 300 -resize "$dim" "$BASE-$dim.png"
done
