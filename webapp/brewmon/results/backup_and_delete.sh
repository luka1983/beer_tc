#!/usr/bin/env bash

suffix=$(date +"%m_%d_%H-%M")

files_list=(ts co t1 t2)

mkdir -p backups

for file in "${files_list[@]}";do
	mv $file backups/${file}.$suffix
done
