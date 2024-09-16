#!/bin/bash

for i in {0..60}
do
    underscores=$(printf '_%.0s' $(seq 1 $i))  # Generate underscores based on i
    nick="Coucou$underscores"
    str="PASS 2\r\nNICK $nick\r\nUSER test$i test$i test$i :test$i\r\nJOIN #chan\r\n"
    echo -e $str | nc 10.11.6.6 8000 &
    sleep 0.1
done