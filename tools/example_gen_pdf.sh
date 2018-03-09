#!/bin/bash

pandoc --listings -H pandoc-fixes.tex --toc -o "$(basename "$1" .md).pdf" "$1" 
