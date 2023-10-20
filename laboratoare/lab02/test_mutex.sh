#!/bin/bash

if [ ! -f "mutex" ]
then
    echo "Nu exista binarul mutex"
    exit
fi

for i in {1..100000}; do ./mutex; done