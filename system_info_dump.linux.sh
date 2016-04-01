#!/bin/bash
echo "----system info---"
uname -a
echo ""
echo "----cpu info---"
cat /proc/cpuinfo | grep vendor | uniq
cat /proc/cpuinfo | grep 'model name' | uniq
cat /proc/cpuinfo | grep -i mhz | uniq
echo ""
echo "----high resolution timer---"
cat /proc/timer_list | grep resolution | sort | uniq
echo ""
echo "----compiler version---"
c++ --version


