#pragma once
#include <vector>
#include <openfhe.h>
#include <chrono>

using namespace lbcrypto;
using namespace std;
using namespace std::chrono;

extern CryptoContext<DCRTPoly> cc;
extern vector<int64_t> times;
