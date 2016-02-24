//
//  ExperienceItem.cpp
//  QuadrocopterBrain
//
//  Created by anton on 30/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "ExperienceItem.hpp"

ExperienceItem::ExperienceItem (
	const Observation& prevState,
	const Observation& nextState,
	double reward,
	long action
):

prevState(prevState),
nextState(nextState),
reward(reward),
action(action)

{}

ExperienceItem::ExperienceItem (
	const Observation& prevState,
	const Observation& nextState,
	double reward,
	const std::vector<float>& actionsScores
):

prevState(prevState),
nextState(nextState),
reward(reward),
actionsScores(actionsScores)

{}
