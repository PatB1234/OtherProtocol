#include <iostream>
#include <vector>
#include <chrono>
#include "master.h"

using namespace std;
using namespace std::chrono;

KeyPair<DCRTPoly> kp1;
time_point<steady_clock, steady_clock::duration> start; // Variables to calculate and record time taken

void generateClientKeys() {
    kp1 = cc->KeyGen();
    cc->EvalMultKeyGen(kp1.secretKey);
}

Ciphertext<DCRTPoly> encryptDataClient(vector<int64_t> data) {
    if (!kp1.publicKey) {
        generateClientKeys();
    }
    Plaintext ptxt = cc->MakePackedPlaintext(data);
    return cc->Encrypt(kp1.publicKey, ptxt);
}

void receive_data(vector<int64_t> data) {
    start = high_resolution_clock::now();
    if (!kp1.publicKey) {
        generateClientKeys();
    }

    constexpr uint64_t modulus = 131071;
    constexpr int64_t shift = modulus / 2;

    vector<Ciphertext<DCRTPoly>> ciphertexts;
    for (int64_t x : data) {
        Plaintext pt = cc->MakeCoefPackedPlaintext(vector<int64_t>{x});
        ciphertexts.push_back(cc->Encrypt(kp1.publicKey, pt));
    }

    auto ctTotal = ciphertexts[0];

    for (int j = 0; j < 3; j++) {
        for (size_t i = 1; i < ciphertexts.size(); i++) {
            for (int r = 0; r < 20; r++) {
                ctTotal = cc->EvalAdd(ctTotal, ciphertexts[i]);
            }
        }
        for (size_t i = 0; i < ciphertexts.size(); i++) {
            for (int r = 0; r < 20; r++) {
                ctTotal = cc->EvalSub(ctTotal, ciphertexts[i]);
            }
        }
    }

    for (size_t i = 1; i < ciphertexts.size(); i++) {
        for (int r = 0; r < 20; r++) {
            ctTotal = cc->EvalAdd(ctTotal, ciphertexts[i]);
        }
    }

    for (size_t i = 0; i < ciphertexts.size(); i++) {
        for (size_t j = 0; j < ciphertexts.size(); j++) {
            ctTotal = cc->EvalSub(ctTotal, ciphertexts[i]);
        }
    }


    Plaintext resultPT;
    cc->Decrypt(kp1.secretKey, ctTotal, &resultPT);

    const vector<int64_t>& totalShifted = resultPT->GetCoefPackedValue();
    int64_t totalSigned = ((totalShifted.at(0) - shift) + modulus) % modulus - shift;
    vector<int64_t> resultVector{totalSigned};

    auto end = steady_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Time Taken: " << duration.count() << " ms" << endl;
    times.push_back(duration.count());
}
