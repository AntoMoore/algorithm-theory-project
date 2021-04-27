#!/bin/bash

echo "Checking file input.txt"
myout="$(./sha512 input.txt)  input.txt"
expected="$(sha512sum input.txt)"
echo "Expected: " $expected
echo "Output: " $myout

if [[ $myout == $expected ]]; then
	echo "Pass"
else
	echo "Fail"
fi