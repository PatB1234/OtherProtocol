#pragma once
#include <vector>
#include <openfhe.h>
#include <chrono>

using namespace std;
using namespace lbcrypto;
using namespace std::chrono;

void receive_data(vector<int64_t> data);
extern time_point<steady_clock, steady_clock::duration> start;
