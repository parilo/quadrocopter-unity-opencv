//
//  QuadrocopterBrain.cpp
//  QuadrocopterBrain
//
//  Created by anton on 19/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "QuadrocopterBrain.hpp"

QuadrocopterBrain::QuadrocopterBrain () {
	experience.reserve(maxExperience);
}

void QuadrocopterBrain::setState (const Observation& state) {
	currentState = state;
}

void QuadrocopterBrain::act () {
	std::cerr << "--- act: " << actExecuted << std::endl;
	
	choosedAction = brain.control(currentState);
//	if (actExecuted % 4000 < 3000) {
		train();
//	}
	
	actExecuted++;
}

void QuadrocopterBrain::train () {
	if (
		actExecuted % trainEveryNth != 0 ||
		experience.size() < 200
	) return;
	
	brain.train(experience);
}

long QuadrocopterBrain::getAction () {
	return choosedAction;
}

void QuadrocopterBrain::storeExperience (const ExperienceItem& expItem) {
	if (actExecuted % storeEveryNth != 0) return;

	if (experience.size() < maxExperience) {
		experience.push_back(expItem);
	} else {
		experience [experienceI] = expItem;
		experienceI++;
		experienceI %= maxExperience;
	}
	
	std::cerr << "--- storeExperience: " << experienceI << std::endl;
}
