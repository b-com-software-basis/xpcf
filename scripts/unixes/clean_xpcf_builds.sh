#!/bin/bash

for i in $(find . -type d -name build-\*)
do
	echo "Removing $i folder"
	rm -rf $i
done
