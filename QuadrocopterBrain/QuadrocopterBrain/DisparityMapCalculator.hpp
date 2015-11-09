//
//  DisparityMapCalculator.hpp
//  QuadrocopterBrain
//
//  Created by anton on 08.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#ifndef DisparityMapCalculator_hpp
#define DisparityMapCalculator_hpp

#include <opencv2/opencv.hpp>

class DisparityMapCalculator {
public:

	DisparityMapCalculator ();
	void set (
		cv::Mat camera1Matrix,
		cv::Mat camera2Matrix,
		cv::Mat camera1distCoeff,
		cv::Mat camera2distCoeff,
		cv::Mat rotationMatrix,
		cv::Mat translationVector,
		cv::Size imageSize
	);
	void compute (
		const cv::Mat& image1,
		const cv::Mat& image2,
		cv::Mat& image1recified,
		cv::Mat& image2recified,
		cv::Mat& disparityMap
	);

private:

	cv::Ptr<cv::StereoSGBM> sgbm;
	cv::Mat camera1Matrix;
	cv::Mat camera2Matrix;
	cv::Mat camera1distCoeff;
	cv::Mat camera2distCoeff;
	cv::Mat rotationMatrix;
	cv::Mat translationVector;
	cv::Size imageSize;
	cv::Mat map11;
	cv::Mat map12;
	cv::Mat map21;
	cv::Mat map22;
    cv::Rect roi1, roi2;
    cv::Mat Q;
	cv::Mat R1;
	cv::Mat P1;
	cv::Mat R2;
	cv::Mat P2;

};

#endif /* DisparityMapCalculator_hpp */
