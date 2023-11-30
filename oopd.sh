#!/bin/bash

# Create a function for creating files
create_files() {
    local dir=$1
    local num_files=$2
    local file_size=$3

    mkdir -p $  dir
    for ((i=0; i<$num_files; i++)); do
        fallocate -l $file_size $dir/file$i
    done
}

# Create a function for creating files recursively
create_files_recursive() {
    local dir=$1
    local num_files=$2
    local file_size=$3
    local total_files=$4

    local count=0
    while ((count < total_files)); do
        create_files $dir $num_files $file_size
        dir=$dir/subdir
        count=$((count + num_files))
    done
}

echo "Creating 100 files of 1GB each..."
time create_files dir1 100 1G

echo "Creating 10000 files of 10MB each..."
time create_files dir2 10000 10M

echo "Creating files recursively until a total of 10000 files are created..."
time create_files_recursive dir3 100 10M 10000
