#!/bin/bash


if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file_path> "
    exit 1
fi


file_path="$1"



if [ ! -f "$file_path" ]; then
    echo "File does not exist."
    exit 1
fi
chmod +r $file_path


lines=$(wc -l < "$file_path")
words=$(wc -w < "$file_path")
chars=$(wc -m < "$file_path")

if [ "$lines" -lt 4 ]; then
  if [ "$words" -gt 1000 ]; then
    if [ "$chars" -gt 2000 ]; then
      echo "$file_path"
      exit 0
    fi
  fi
fi
if grep -q -P '[^\x00-\x7F]' "$file_path"; then

    echo "$file_path"
    exit 0
fi


words_to_search=("corrupted" "dangerous" "risk" "attack" "malware" "malicious")


for word in "${words_to_search[@]}"; do
    if grep -q "$word" "$file_path"; then

        echo "$file_path"
        exit 0
    fi
done


echo "SAFE"

