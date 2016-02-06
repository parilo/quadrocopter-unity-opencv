//
//  Observation.hpp
//  QuadrocopterBrain
//
//  Created by anton on 21/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#ifndef Observation_hpp
#define Observation_hpp

class Observation {

public:

	Observation ();

	Observation (
		double currentRotW,
		double currentRotX,
		double currentRotY,
		double currentRotZ,

//		double currentX,
//		double currentY,
//		double currentZ,

		double targetX,
		double targetY,
		double targetZ,

		double motor1power,
		double motor2power,
		double motor3power,
		double motor4power
	);

	void set (
		double currentRotW,
		double currentRotX,
		double currentRotY,
		double currentRotZ,

//		double targetRotW,
//		double targetRotX,
//		double targetRotY,
//		double targetRotZ,

//		double currentX,
//		double currentY,
//		double currentZ,

		double targetX,
		double targetY,
		double targetZ,

		double motor1power,
		double motor2power,
		double motor3power,
		double motor4power
	);

	double currentRotW;
	double currentRotX;
	double currentRotY;
	double currentRotZ;

//	double targetRotW;
//	double targetRotX;
//	double targetRotY;
//	double targetRotZ;

//	double currentX;
//	double currentY;
//	double currentZ;

	double targetX;
	double targetY;
	double targetZ;

	double motor1power;
	double motor2power;
	double motor3power;
	double motor4power;
	
	int getSize () const { return 11; }
	void print () const;

};

#endif /* Observation_hpp */
