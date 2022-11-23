This is a project containing some sample codes for CTP API in linux.
API version: v6.6.7_CP_20220304

./login: containing a simple example of login in for trader API.
./common: containing common scripts used for converting string of Chinese characters and show the characters in the cmd correctly.
./api: CTP API files.

./tradeapitest: implement more complicated strategies using CTP API.
./tradeapitest/trader_test: testing code for different trader and md API functions.
./tradeapitest/common.h: common headers across scripts.
./mdhandler.cpp: override response functions for md API.
./traderandler.cpp: override response functions for trader API.
./utils.cpp: aggregate functions calls that have been implemented in the original trader and MD API.

./run.sh: run scripts (currently running trader_arbitrage scripts)