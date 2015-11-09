//
//  StereoCalibrator.hpp
//  QuadrocopterBrain
//
//  Created by anton on 09.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#ifndef StereoCalibrator_hpp
#define StereoCalibrator_hpp

#include <opencv2/opencv.hpp>

class StereoCalibrator {
public:

	void set (
		cv::Size boardSize,
		double squareSize,
		cv::Size imageSize
	);
	
	void makeCalibration (
		const std::vector<std::vector<cv::Point2f>>& camera1SamplesPoints,
		const std::vector<std::vector<cv::Point2f>>& camera2SamplesPoints,
		const cv::Mat& camera1Matrix,
		const cv::Mat& camera1DistCoeffs,
		const cv::Mat& camera2Matrix,
		const cv::Mat& camera2DistCoeffs
	);
	
    const cv::Mat& getRotationMatrix ();
	const cv::Mat& getTranslationVector ();
	const cv::Mat& getEssentialMatrix ();
	const cv::Mat& getFundamentalMatrix ();
	
	void clear ();

private:

	cv::Size imageSize;
	std::vector<cv::Point3f> chessboardCorners;
    cv::Mat rotationMatrix;
	cv::Mat translationVector;
	cv::Mat essentialMatrix;
	cv::Mat fundamentalMatrix;

};

#endif /* StereoCalibrator_hpp */
