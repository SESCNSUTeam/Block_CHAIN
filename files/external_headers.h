#pragma once
#define WIN32_LEAN_AND_MEAN

#pragma comment (lib, "Ws2_32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>
#include "sha.h"
#include "hex.h"
#include "dsa.h"
#include "osrng.h"
