//
//  Lib.hpp
//  QuadrocopterBrain
//
//  Created by anton on 21/01/16.
//  Copyright © 2016 anton. All rights reserved.
//

#ifndef Lib_hpp
#define Lib_hpp

class Lib {

public:
	
	static void randomize ();
	static int randInt (int min, int max);
	static float randFloat (float LO, float HI);
	static double randDouble (double LO, double HI);
	static long long getTimestampInMillis ();
	
};

#endif /* Lib_hpp */
