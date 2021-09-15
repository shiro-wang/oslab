#!/bin/sh

echo "Welcome to the Number Guessing Game!"
echo "Please enter the target number: $1"
b=1
t=10
pass(){
	echo "Please enter a number between $b and $t:"
}
i=0
ans=$1
next(){
	if [ $i -lt $ans ]; then
		b=`expr $i + 1`
	else
		t=`expr $i - 1`
	fi
}
while [ $i -ne $ans ]; do
	echo here~~
	pass;
	read i
	next;
done

echo "Bingo!"
