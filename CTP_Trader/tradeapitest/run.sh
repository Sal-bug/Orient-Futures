#!/bin/bash
echo "200718" | sudo -S chmod +777 /sys/firmware/dmi/tables/smbios_entry_point
echo "200718" | sudo -S chmod +777 /sys/firmware/dmi/tables/DMI
g++ -c *.cpp ./trader_arbitrage/*.cpp
g++ *.o ../common/code_convert.o ../api/thosttraderapi_se.so ../api/thostmduserapi_se.so -o test
./test
