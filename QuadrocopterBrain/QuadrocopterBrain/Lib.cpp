//
//  Lib.cpp
//  QuadrocopterBrain
//
//  Created by anton on 21/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "Lib.hpp"
#include <cstdlib>
#include <ctime>
#include <chrono>

void Lib::randomize () {
    std::srand ((unsigned int)time(NULL));
}

int Lib::randInt (int min, int max) {
    return min + (std::rand() % (int)(max - min + 1));
}

float Lib::randFloat (float LO, float HI) {
    return LO + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}

double Lib::randDouble (double LO, double HI) {
    return LO + static_cast <double> (std::rand()) /( static_cast <double> (RAND_MAX/(HI-LO)));
}

long long Lib::getTimestampInMillis () {
    using namespace std::chrono;
    return duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count();
}
