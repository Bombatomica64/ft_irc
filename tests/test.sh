#!/bin/bash

for i in {1..60}
do
	str="PASS 2\r\nNICK test$i\r\nUSER test$i test$i test$i :test$i\r\nJOIN #chan\r\n"
	echo -e $str | nc 10.11.6.6 8000 &
	sleep 0.1
done
