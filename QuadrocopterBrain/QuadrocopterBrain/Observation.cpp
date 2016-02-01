//
//  Observation.cpp
//  QuadrocopterBrain
//
//  Created by anton on 21/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "Observation.hpp"
#include <iostream>

Observation::Observation ():
currentRotW (1),
currentRotX (0),
currentRotY (0),
currentRotZ (0),

currentX (0),
currentY (0),
currentZ (0),

targetX (0),
targetY (0),
targetZ (0),

motor1power (0),
motor2power (0),
motor3power (0),
motor4power (0)

{}

Observation::Observation (
	double currentRotW,
	double currentRotX,
	double currentRotY,
	double currentRotZ,

	double currentX,
	double currentY,
	double currentZ,

	double targetX,
	double targetY,
	double targetZ,

	double motor1power,
	double motor2power,
	double motor3power,
	double motor4power
):

currentRotW (currentRotW),
currentRotX (currentRotX),
currentRotY (currentRotY),
currentRotZ (currentRotZ),

currentX (currentX),
currentY (currentY),
currentZ (currentZ),

targetX (targetX),
targetY (targetY),
targetZ (targetZ),

motor1power (motor1power),
motor2power (motor2power),
motor3power (motor3power),
motor4power (motor4power)

{}


void Observation::set (
	double currentRotW,
	double currentRotX,
	double currentRotY,
	double currentRotZ,

//	double targetRotW,
//	double targetRotX,
//	double targetRotY,
//	double targetRotZ,

	double currentX,
	double currentY,
	double currentZ,

	double targetX,
	double targetY,
	double targetZ,

	double motor1power,
	double motor2power,
	double motor3power,
	double motor4power
) {

	this->currentRotW = currentRotW;
	this->currentRotX = currentRotX;
	this->currentRotY = currentRotY;
	this->currentRotZ = currentRotZ;
	
//	this->targetRotW = targetRotW;
//	this->targetRotX = targetRotX;
//	this->targetRotY = targetRotY;
//	this->targetRotZ = targetRotZ;
	
	this->currentX = currentX;
	this->currentY = currentY;
	this->currentZ = currentZ;
	
	this->targetX = targetX;
	this->targetY = targetY;
	this->targetZ = targetZ;
	
	this->motor1power = motor1power;
	this->motor2power = motor2power;
	this->motor3power = motor3power;
	this->motor4power = motor4power;

}

void Observation::print () const {
	std::cerr <<
	this->currentRotW << " " <<
	this->currentRotX << " " <<
	this->currentRotY << " " <<
	this->currentRotZ << " " <<
	
	this->currentX << " " <<
	this->currentY << " " <<
	this->currentZ << " " <<
	
	this->targetX << " " <<
	this->targetY << " " <<
	this->targetZ << " " <<
	
	this->motor1power << " " <<
	this->motor2power << " " <<
	this->motor3power << " " <<
	this->motor4power << std::endl;
}
