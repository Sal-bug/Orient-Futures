This is a project containing some sample codes for CTP API in linux.
API version: v6.6.7_CP_20220304
Using SimNow testing account

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

./trader_arbitrage: include two arbitrage strategies: calendar spread and butterfly spread
./trader_arbitrage/arbitrage_utils.cpp: define classes and functions for the two arbitrage strategies

Notes:
Both the arbitrage cannot be tested currently but basic logics are implemented.
The unfinished part is labeled with comments `TODO`.

Market data cannot be accessed for some bid price and ask price. 
Also the market data is updating very slowly which cannot trigger the operation of opening positions.
The total number of instruments available is very small.

Changing accounts can cause compatibility problems with specific versions of API.
The account can be in compatible with certain functions of certain versions of API.
