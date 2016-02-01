//
//  QuadrocopterBrain.cpp
//  QuadrocopterBrain
//
//  Created by anton on 19/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "QuadrocopterBrain.hpp"

void QuadrocopterBrain::setState (const Observation& state) {
	currentState = state;
}

void QuadrocopterBrain::act () {
	std::cerr << "--- act" << std::endl;
	
	choosedAction = brain.control(currentState);
	train();
	
	actExecuted++;
}

void QuadrocopterBrain::train () {
	if (actExecuted % trainEveryNth != 0) return;
	
	brain.train(experience);
}

long QuadrocopterBrain::getAction () {
	return choosedAction;
}

void QuadrocopterBrain::storeExperience (const ExperienceItem& expItem) {
	if (actExecuted % storeEveryNth != 0) return;
	
	experience.push_back(expItem);
	if (experience.size() > maxExperience) experience.pop_front();
	std::cerr << "--- storeExperience: " << experience.size() << std::endl;
}
