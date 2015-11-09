//
//  CameraUndistortion.cpp
//  QuadrocopterBrain
//
//  Created by anton on 08.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#include "CameraUndistortion.hpp"

#include "opencv2/calib3d.hpp"

using namespace cv;
using namespace std;

void CameraUndistortion::set (const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const cv::Size& imageSize) {
	this->cameraMatrix = cameraMatrix;
	this->distCoeffs = distCoeffs;
	this->imageSize = imageSize;
	
	initUndistortRectifyMap(
		cameraMatrix, distCoeffs, Mat(),
		getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0), imageSize,
		CV_16SC2, map1, map2);
}

void CameraUndistortion::undistort (const cv::Mat& srcImage, cv::Mat& undistortedImage) {
	remap(srcImage, undistortedImage, map1, map2, INTER_LINEAR);
}
