#!/bin/sh

(
echo "reset halt"
sleep 1
echo "flash write_image erase $1 0x08000000"
sleep 4
echo "reset run"
sleep 1
echo "exit"
) | telnet localhost 4444
