//
//  CameraCalibrator.hpp
//  QuadrocopterBrain
//
//  Created by anton on 07.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#ifndef CameraCalibrator_hpp
#define CameraCalibrator_hpp

#include <opencv2/opencv.hpp>

class CameraCalibrator {
public:

	void set (
		cv::Size boardSize,
		double squareSize,
		cv::Size imageSize
	);
	
	void findSample (const cv::Mat& img);
	bool isSampleFound ();
	void acceptSample ();
	
	void makeCalibration ();
	void clear ();
	const std::vector<cv::Point2f>& getLastSamplePoints ();
	const std::vector<std::vector<cv::Point2f>>& getSamplesPoints ();
	
	const cv::Mat& getCameraMatrix ();
	const cv::Mat& getDistCoeffs ();

private:

	cv::Size boardSize;
	float squareSize;
	cv::Size imageSize;
	
	std::vector<std::vector<cv::Point2f>> samplesPoints;
	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;

	std::vector<cv::Point2f> currentSamplePoints;
	bool sampleFound;
	const cv::Mat* currentImage;

};

void calcChessboardCorners(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners);

#endif /* CameraCalibrator_hpp */
