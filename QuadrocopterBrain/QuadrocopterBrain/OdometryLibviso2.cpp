//
//  OdometryLibviso2.cpp
//  QuadrocopterBrain
//
//  Created by anton on 25.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#include "OdometryLibviso2.hpp"
#include "Main.h"

using namespace cv;
using namespace std;

OdometryLibviso2::OdometryLibviso2 () : viso(param) {
}

void OdometryLibviso2::set (const Mat& camera, const Mat& T) {

	param.calib.f  = camera.at<double>(0, 0); // focal length in pixels
	param.calib.cu = camera.at<double>(0, 2); // principal point (u-coordinate) in pixels
	param.calib.cv = camera.at<double>(1, 2); // principal point (v-coordinate) in pixels
	param.base     = sqrt(T.dot(T)); // baseline in meters
	
	stringstream outs;
	outs << "--- libviso2 set: " <<
	"f: " << param.calib.f << endl <<
	"cu: " << param.calib.cu << endl <<
	"cv: " << param.calib.cv << endl <<
	"base: " << param.base << endl;
	DebugLog(outs.str());
	
	viso = VisualOdometryStereo (param);
	
	// current pose (this matrix transforms a point from the current
	// frame's camera coordinates to the first frame's camera coordinates)
	pose = Matrix::eye(4);
}

void OdometryLibviso2::calcOdometry (const Mat& leftGray, const Mat& rightGray) {

      // image dimensions
      int32_t width  = leftGray.cols;
      int32_t height = leftGray.rows;

      // convert input images to uint8_t buffer
      uint8_t* left_img_data  = (uint8_t*)malloc(width*height*sizeof(uint8_t));
      uint8_t* right_img_data = (uint8_t*)malloc(width*height*sizeof(uint8_t));
      int32_t k=0;
      for (int32_t v=0; v<height; v++) {
        for (int32_t u=0; u<width; u++) {
          left_img_data[k]  = leftGray.at<uint8_t>(u, v);
          right_img_data[k] = rightGray.at<uint8_t>(u, v);
          k++;
        }
      }

		stringstream cout;
		cout << "--- calc odometry: ";
	
      // compute visual odometry
      int32_t dims[] = {width,height,width};
      if (viso.process(left_img_data,right_img_data,dims)) {
      
        // on success, update current pose
        pose = pose * Matrix::inv(viso.getMotion());
      
        // output some statistics
        double num_matches = viso.getNumberOfMatches();
        double num_inliers = viso.getNumberOfInliers();
		
        cout << " Matches: " << num_matches;
        cout << " Inliers: " << 100.0*num_inliers/num_matches << " %" << ", Current pose: " << endl;
        cout << pose << endl << endl;

      } else {
        cout << " ... failed!" << endl;
      }
	
	  DebugLog(cout.str());

      // release uint8_t buffers
      free(left_img_data);
      free(right_img_data);

}

