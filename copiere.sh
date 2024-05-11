#!/bin/bash


if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <source_folder> <file_name> <destination_folder>"
    exit 1
fi


source_folder="$1"
file_name="$2"
destination_folder="$3"

touch "$destination_folder/$file_name"


cp "$source_folder" "$destination_folder/$file_name"


