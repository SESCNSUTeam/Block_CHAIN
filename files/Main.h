#pragma once

#define WIN32_LEAN_AND_MEAN


#include <iostream>
#include <math.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>


#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <chrono>
#include <map>

#include "Crypto.h"
#include "Block.h"
#include "sha.h"
#include "hex.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "Block.h"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "43012"


Block createBlock(Block curr, int cp);
double getHashSpeed();
bool if_pow(string data, int c);
double getComputationPower(float time, int power);