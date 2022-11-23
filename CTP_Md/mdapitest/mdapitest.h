
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>

//#include <FtdcTraderApi.h>

#include "../api/ThostFtdcTraderApi.h"
#include "../api/ThostFtdcMdApi.h"
#include "../api/ThostFtdcUserApiDataType.h"
#include "../api/ThostFtdcUserApiStruct.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "../common/code_convert.h"
