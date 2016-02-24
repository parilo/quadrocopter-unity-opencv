//
//  ExperienceItem.hpp
//  QuadrocopterBrain
//
//  Created by anton on 30/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#ifndef ExperienceItem_hpp
#define ExperienceItem_hpp

#include "Observation.hpp"

class ExperienceItem {

	public:

	ExperienceItem (
		const Observation& prevState,
		const Observation& nextState,
		double reward,
		long action
	);

	ExperienceItem (
		const Observation& prevState,
		const Observation& nextState,
		double reward,
		const std::vector<float>& actionsScores
	);
	
	Observation prevState;
	Observation nextState;
	double reward;
	long action;
	std::vector<float> actionsScores;

};

#endif /* ExperienceItem_hpp */
