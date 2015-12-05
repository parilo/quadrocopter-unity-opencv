//
//  OdometryLibviso2.hpp
//  QuadrocopterBrain
//
//  Created by anton on 25.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#ifndef OdometryLibviso2_hpp
#define OdometryLibviso2_hpp

#include <opencv2/opencv.hpp>
#include "libviso2/viso_stereo.h"

class OdometryLibviso2 {
public:

	OdometryLibviso2 ();
	
	void set (const cv::Mat& camera, const cv::Mat& T);
	void calcOdometry (const cv::Mat& leftGray, const cv::Mat& rightGray);

private:

	VisualOdometryStereo::parameters param;
	VisualOdometryStereo viso;
	Matrix pose;
	
};

#endif /* OdometryLibviso2_hpp */
