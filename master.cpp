#include <iostream>
#include <openfhe.h>
#include <chrono>

#include "master.h"
#include "iotDevice.h"

using namespace std;
using namespace lbcrypto;
using namespace std::chrono;

// Define the shared context
CryptoContext<DCRTPoly> cc;
vector<int64_t> times;

void InitContext() {
    if (cc) return; // already init

    //Paramater setup
    CCParams<CryptoContextBGVRNS> parameters;
    parameters.SetPlaintextModulus(131071);
    parameters.SetMultiplicativeDepth(3);
    parameters.SetRingDim(16384);

    cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    cc->Enable(PRE);
}

int main() {
    // Load data into queue
    queue<vector<int64_t>> data;
    ifstream file;
    file.open("data.txt");
    string line;
    while (getline (file, line)) {
        stringstream ss(line);
        vector<int64_t> row;
        string token;

        // split by commas
        while (getline(ss, token, ',')) {
            if (!token.empty()) {
                row.push_back(stoll(token));
            }
        }

        if (!row.empty()) {
            data.push(row);
        }
    }

    file.close();

    while (!data.empty()) {
        InitContext();

        // Each party creates its keys in the same context
        //GENERATE KEYS HERE
        auto currData = data.front();
        cout << "IoT device raw data: " << endl;
        for (int i = 0; i < currData.size(); i++) {

            cout << currData[i] << endl;
        }

        // Simulate device->client side action (encrypt & send to server with rk(C->S))
        start = high_resolution_clock::now();
        receive_data(currData);
        data.pop();
    }
    // Calculate average time:
    int64_t sum = 0;
    for (size_t i = 0; i < times.size(); i++) {
        sum += times[i];
    }

    double avg = 0.0;
    if (!times.empty()) {
        avg = static_cast<double>(sum) / times.size();
    }

    cout << "Average time: " <<  avg << " ms" << endl;

    return 0;
}
