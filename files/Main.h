#pragma once


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

Block createBlock(Block curr, int cp);
double getHashSpeed();
bool if_pow(string data, int c);
double getComputationPower(float time, int power);