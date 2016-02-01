//
//  BrainDiscreteDeepQ.hpp
//  QuadrocopterBrain
//
//  Created by anton on 17/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#ifndef BrainDiscreteDeepQ_hpp
#define BrainDiscreteDeepQ_hpp

#include <deque>

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/public/env.h"

#include "Observation.hpp"
#include "ExperienceItem.hpp"

class BrainDiscreteDeepQ {

public:

	BrainDiscreteDeepQ ();

	/**
		Returns index of action
	*/
	long control (const Observation& ob);
	
	void train (const std::deque<ExperienceItem>& experience);
	
private:

	int actionsExecutedSoFar = 0;
	
	//linear annealing parameters
	int explorationPeriod = 5000;
	double randomActionProbabilityInitial = 1.0;
	double randomActionProbabilityFinal = 0.05;
	
	int numActions = 12;
	
	//train
	int minibatchSize = 32;
	
	tensorflow::Session* session;
	
	double linearAnnealing();
	

};

#endif /* BrainDiscreteDeepQ_hpp */
