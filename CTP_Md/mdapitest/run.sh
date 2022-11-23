#!/bin/bash
g++ -c *.cpp
g++ *.o ../common/code_convert.o ../api/thostmduserapi_se.so -o test
./test