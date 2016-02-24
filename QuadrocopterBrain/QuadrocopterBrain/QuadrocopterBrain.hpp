//
//  QuadrocopterBrain.hpp
//  QuadrocopterBrain
//
//  Created by anton on 19/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#ifndef QuadrocopterBrain_hpp
#define QuadrocopterBrain_hpp

#include <deque>

#include "BrainDiscreteDeepQ.hpp"
#include "ExperienceItem.hpp"

class QuadrocopterBrain {

public:
	
	QuadrocopterBrain ();
	
	void setState (const Observation& state);
	
	void act ();
	
	long getAction ();
	const std::vector<float>& getActionsScores ();
	
	void storeExperience (const ExperienceItem& expItem);
	
	void train ();
	
private:

	long choosedAction;
	std::vector<float> actionsScores;

	Observation currentState;
	BrainDiscreteDeepQ brain;
	std::vector<ExperienceItem> experience;
	int experienceI = 0;

	long actExecuted = 0;
	int storeEveryNth = 1;
	int trainEveryNth = 2000;
	int trainCount = 2000;
	int maxExperience = 30000;

};

#endif /* QuadrocopterBrain_hpp */
