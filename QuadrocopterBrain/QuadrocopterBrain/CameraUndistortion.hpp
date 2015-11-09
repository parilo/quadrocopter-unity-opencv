//
//  CameraUndistortion.hpp
//  QuadrocopterBrain
//
//  Created by anton on 08.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#ifndef CameraUndistortion_hpp
#define CameraUndistortion_hpp

#include <opencv2/opencv.hpp>

class CameraUndistortion {
public:

	void set (const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, const cv::Size& imageSize);
	void undistort (const cv::Mat& srcImage, cv::Mat& undistortedImage);

private:

	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;
	cv::Size imageSize;
	cv::Mat map1;
	cv::Mat map2;

};

#endif /* CameraUndistortion_hpp */
