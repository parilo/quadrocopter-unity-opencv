//
//  DisparityMapCalculator.cpp
//  QuadrocopterBrain
//
//  Created by anton on 08.11.15.
//  Copyright © 2015 anton. All rights reserved.
//

#include "DisparityMapCalculator.hpp"

using namespace cv;
using namespace std;

void DisparityMapCalculator::set (
	cv::Mat camera1Matrix,
	cv::Mat camera2Matrix,
	cv::Mat camera1distCoeff,
	cv::Mat camera2distCoeff,
	cv::Mat rotationMatrix,
	cv::Mat translationVector,
	cv::Size imageSize
) {
	this->camera1Matrix = camera1Matrix;
	this->camera2Matrix = camera2Matrix;
	this->camera1distCoeff = camera1distCoeff;
	this->camera2distCoeff = camera2distCoeff;
	this->rotationMatrix = rotationMatrix;
	this->translationVector = translationVector;
	
	stereoRectify( camera1Matrix, camera1distCoeff, camera2Matrix, camera2distCoeff, imageSize, rotationMatrix, translationVector, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, imageSize, &roi1, &roi2 );
	
	initUndistortRectifyMap(camera1Matrix, camera1distCoeff, R1, P1, imageSize, CV_16SC2, map11, map12);
	initUndistortRectifyMap(camera2Matrix, camera2distCoeff, R2, P2, imageSize, CV_16SC2, map21, map22);

}

DisparityMapCalculator::DisparityMapCalculator () {
	sgbm = StereoSGBM::create(0,16,3);
}

void DisparityMapCalculator::compute (
	const cv::Mat& image1,
	const cv::Mat& image2,
	cv::Mat& image1recified,
	cv::Mat& image2recified,
	cv::Mat& disparityMap
) {

	Mat img1, img2;
	remap(image1, image1recified, map11, map12, INTER_LINEAR);
	remap(image2, image2recified, map21, map22, INTER_LINEAR);

    int SADWindowSize = 0, numberOfDisparities = 0;
	
    sgbm->setPreFilterCap(63);
    int sgbmWinSize = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize(sgbmWinSize);

    int cn = image1.channels();
    Size img_size = image1.size();
    numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((img_size.width/8) + 15) & -16;

    sgbm->setP1(8*cn*sgbmWinSize*sgbmWinSize);
    sgbm->setP2(32*cn*sgbmWinSize*sgbmWinSize);
    sgbm->setMinDisparity(0);
    sgbm->setNumDisparities(numberOfDisparities);
    sgbm->setUniquenessRatio(10);
    sgbm->setSpeckleWindowSize(100);
    sgbm->setSpeckleRange(32);
    sgbm->setDisp12MaxDiff(1);
    sgbm->setMode(StereoSGBM::MODE_SGBM);
//    sgbm->setMode(alg == STEREO_HH ? StereoSGBM::MODE_HH : StereoSGBM::MODE_SGBM);

	sgbm->compute(image1, image2, disparityMap);
}































///* This is sample from the OpenCV book. The copyright notice is below */
//
///* *************** License:**************************
//   Oct. 3, 2008
//   Right to use this code in any way you want without warranty, support or any guarantee of it working.
//
//   BOOK: It would be nice if you cited it:
//   Learning OpenCV: Computer Vision with the OpenCV Library
//     by Gary Bradski and Adrian Kaehler
//     Published by O'Reilly Media, October 3, 2008
//
//   AVAILABLE AT:
//     http://www.amazon.com/Learning-OpenCV-Computer-Vision-Library/dp/0596516134
//     Or: http://oreilly.com/catalog/9780596516130/
//     ISBN-10: 0596516134 or: ISBN-13: 978-0596516130
//
//   OPENCV WEBSITES:
//     Homepage:      http://opencv.org
//     Online docs:   http://docs.opencv.org
//     Q&A forum:     http://answers.opencv.org
//     Issue tracker: http://code.opencv.org
//     GitHub:        https://github.com/Itseez/opencv/
//   ************************************************** */
//
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//
//#include <vector>
//#include <string>
//#include <algorithm>
//#include <iostream>
//#include <iterator>
//#include <stdio.h>
//#include <stdlib.h>
//#include <ctype.h>
//
//using namespace cv;
//using namespace std;
//
//static int print_help()
//{
//    cout <<
//            " Given a list of chessboard images, the number of corners (nx, ny)\n"
//            " on the chessboards, and a flag: useCalibrated for \n"
//            "   calibrated (0) or\n"
//            "   uncalibrated \n"
//            "     (1: use cvStereoCalibrate(), 2: compute fundamental\n"
//            "         matrix separately) stereo. \n"
//            " Calibrate the cameras and display the\n"
//            " rectified results along with the computed disparity images.   \n" << endl;
//    cout << "Usage:\n ./stereo_calib -w board_width -h board_height [-nr /*dot not view results*/] <image list XML/YML file>\n" << endl;
//    return 0;
//}
//
//
//static void
//StereoCalib(const vector<string>& imagelist, Size boardSize, bool useCalibrated=true, bool showRectified=true)
//{
//    if( imagelist.size() % 2 != 0 )
//    {
//        cout << "Error: the image list contains odd (non-even) number of elements\n";
//        return;
//    }
//
//    bool displayCorners = false;//true;
//    const int maxScale = 2;
//    const float squareSize = 1.f;  // Set this to your actual square size
//    // ARRAY AND VECTOR STORAGE:
//
//    vector<vector<Point2f> > imagePoints[2];
//    vector<vector<Point3f> > objectPoints;
//    Size imageSize;
//
//    int i, j, k, nimages = (int)imagelist.size()/2;
//
//    imagePoints[0].resize(nimages);
//    imagePoints[1].resize(nimages);
//    vector<string> goodImageList;
//
//    for( i = j = 0; i < nimages; i++ )
//    {
//        for( k = 0; k < 2; k++ )
//        {
//            const string& filename = imagelist[i*2+k];
//            Mat img = imread(filename, 0);
//            if(img.empty())
//                break;
//            if( imageSize == Size() )
//                imageSize = img.size();
//            else if( img.size() != imageSize )
//            {
//                cout << "The image " << filename << " has the size different from the first image size. Skipping the pair\n";
//                break;
//            }
//            bool found = false;
//            vector<Point2f>& corners = imagePoints[k][j];
//            for( int scale = 1; scale <= maxScale; scale++ )
//            {
//                Mat timg;
//                if( scale == 1 )
//                    timg = img;
//                else
//                    resize(img, timg, Size(), scale, scale);
//                found = findChessboardCorners(timg, boardSize, corners,
//                    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
//                if( found )
//                {
//                    if( scale > 1 )
//                    {
//                        Mat cornersMat(corners);
//                        cornersMat *= 1./scale;
//                    }
//                    break;
//                }
//            }
//            if( displayCorners )
//            {
//                cout << filename << endl;
//                Mat cimg, cimg1;
//                cvtColor(img, cimg, COLOR_GRAY2BGR);
//                drawChessboardCorners(cimg, boardSize, corners, found);
//                double sf = 640./MAX(img.rows, img.cols);
//                resize(cimg, cimg1, Size(), sf, sf);
//                imshow("corners", cimg1);
//                char c = (char)waitKey(500);
//                if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
//                    exit(-1);
//            }
//            else
//                putchar('.');
//            if( !found )
//                break;
//            cornerSubPix(img, corners, Size(11,11), Size(-1,-1),
//                         TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
//                                      30, 0.01));
//        }
//        if( k == 2 )
//        {
//            goodImageList.push_back(imagelist[i*2]);
//            goodImageList.push_back(imagelist[i*2+1]);
//            j++;
//        }
//    }
//    cout << j << " pairs have been successfully detected.\n";
//    nimages = j;
//    if( nimages < 2 )
//    {
//        cout << "Error: too little pairs to run the calibration\n";
//        return;
//    }
//
//    imagePoints[0].resize(nimages);
//    imagePoints[1].resize(nimages);
//    objectPoints.resize(nimages);
//
//    for( i = 0; i < nimages; i++ )
//    {
//        for( j = 0; j < boardSize.height; j++ )
//            for( k = 0; k < boardSize.width; k++ )
//                objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
//    }
//
//    cout << "Running stereo calibration ...\n";
//
//    Mat cameraMatrix[2], distCoeffs[2];
//    cameraMatrix[0] = Mat::eye(3, 3, CV_64F);
//    cameraMatrix[1] = Mat::eye(3, 3, CV_64F);
//    Mat R, T, E, F;
//
//    double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
//                    cameraMatrix[0], distCoeffs[0],
//                    cameraMatrix[1], distCoeffs[1],
//                    imageSize, R, T, E, F,
//                    CALIB_FIX_ASPECT_RATIO +
//                    CALIB_ZERO_TANGENT_DIST +
//                    CALIB_SAME_FOCAL_LENGTH +
//                    CALIB_RATIONAL_MODEL +
//                    CALIB_FIX_K3 + CALIB_FIX_K4 + CALIB_FIX_K5,
//                    TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 100, 1e-5) );
//    cout << "done with RMS error=" << rms << endl;
//
//// CALIBRATION QUALITY CHECK
//// because the output fundamental matrix implicitly
//// includes all the output information,
//// we can check the quality of calibration using the
//// epipolar geometry constraint: m2^t*F*m1=0
//    double err = 0;
//    int npoints = 0;
//    vector<Vec3f> lines[2];
//    for( i = 0; i < nimages; i++ )
//    {
//        int npt = (int)imagePoints[0][i].size();
//        Mat imgpt[2];
//        for( k = 0; k < 2; k++ )
//        {
//            imgpt[k] = Mat(imagePoints[k][i]);
//            undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
//            computeCorrespondEpilines(imgpt[k], k+1, F, lines[k]);
//        }
//        for( j = 0; j < npt; j++ )
//        {
//            double errij = fabs(imagePoints[0][i][j].x*lines[1][j][0] +
//                                imagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
//                           fabs(imagePoints[1][i][j].x*lines[0][j][0] +
//                                imagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
//            err += errij;
//        }
//        npoints += npt;
//    }
//    cout << "average reprojection err = " <<  err/npoints << endl;
//
//    // save intrinsic parameters
//    FileStorage fs("../data/intrinsics.yml", FileStorage::WRITE);
//    if( fs.isOpened() )
//    {
//        fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
//            "M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
//        fs.release();
//    }
//    else
//        cout << "Error: can not save the intrinsic parameters\n";
//
//    Mat R1, R2, P1, P2, Q;
//    Rect validRoi[2];
//
//    stereoRectify(cameraMatrix[0], distCoeffs[0],
//                  cameraMatrix[1], distCoeffs[1],
//                  imageSize, R, T, R1, R2, P1, P2, Q,
//                  CALIB_ZERO_DISPARITY, 1, imageSize, &validRoi[0], &validRoi[1]);
//
//    fs.open("extrinsics.yml", FileStorage::WRITE);
//    if( fs.isOpened() )
//    {
//        fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
//        fs.release();
//    }
//    else
//        cout << "Error: can not save the extrinsic parameters\n";
//
//    // OpenCV can handle left-right
//    // or up-down camera arrangements
//    bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));
//
//// COMPUTE AND DISPLAY RECTIFICATION
//    if( !showRectified )
//        return;
//
//    Mat rmap[2][2];
//// IF BY CALIBRATED (BOUGUET'S METHOD)
//    if( useCalibrated )
//    {
//        // we already computed everything
//    }
//// OR ELSE HARTLEY'S METHOD
//    else
// // use intrinsic parameters of each camera, but
// // compute the rectification transformation directly
// // from the fundamental matrix
//    {
//        vector<Point2f> allimgpt[2];
//        for( k = 0; k < 2; k++ )
//        {
//            for( i = 0; i < nimages; i++ )
//                std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
//        }
//        F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_8POINT, 0, 0);
//        Mat H1, H2;
//        stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);
//
//        R1 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
//        R2 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
//        P1 = cameraMatrix[0];
//        P2 = cameraMatrix[1];
//    }
//
//    //Precompute maps for cv::remap()
//    initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
//    initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
//
//    Mat canvas;
//    double sf;
//    int w, h;
//    if( !isVerticalStereo )
//    {
//        sf = 600./MAX(imageSize.width, imageSize.height);
//        w = cvRound(imageSize.width*sf);
//        h = cvRound(imageSize.height*sf);
//        canvas.create(h, w*2, CV_8UC3);
//    }
//    else
//    {
//        sf = 300./MAX(imageSize.width, imageSize.height);
//        w = cvRound(imageSize.width*sf);
//        h = cvRound(imageSize.height*sf);
//        canvas.create(h*2, w, CV_8UC3);
//    }
//
//    for( i = 0; i < nimages; i++ )
//    {
//        for( k = 0; k < 2; k++ )
//        {
//            Mat img = imread(goodImageList[i*2+k], 0), rimg, cimg;
//            remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
//            cvtColor(rimg, cimg, COLOR_GRAY2BGR);
//            Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
//            resize(cimg, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);
//            if( useCalibrated )
//            {
//                Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
//                          cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
//                rectangle(canvasPart, vroi, Scalar(0,0,255), 3, 8);
//            }
//        }
//
//        if( !isVerticalStereo )
//            for( j = 0; j < canvas.rows; j += 16 )
//                line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
//        else
//            for( j = 0; j < canvas.cols; j += 16 )
//                line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
//        imshow("rectified", canvas);
//        char c = (char)waitKey();
//        if( c == 27 || c == 'q' || c == 'Q' )
//            break;
//    }
//}
//
//
//static bool readStringList( const string& filename, vector<string>& l )
//{
//    l.resize(0);
//    FileStorage fs(filename, FileStorage::READ);
//    if( !fs.isOpened() )
//        return false;
//    FileNode n = fs.getFirstTopLevelNode();
//    if( n.type() != FileNode::SEQ )
//        return false;
//    FileNodeIterator it = n.begin(), it_end = n.end();
//    for( ; it != it_end; ++it )
//        l.push_back((string)*it);
//    return true;
//}
//
//int main(int argc, char** argv)
//{
//    Size boardSize;
//    string imagelistfn;
//    bool showRectified = true;
//
//    for( int i = 1; i < argc; i++ )
//    {
//        if( string(argv[i]) == "-w" )
//        {
//            if( sscanf(argv[++i], "%d", &boardSize.width) != 1 || boardSize.width <= 0 )
//            {
//                cout << "invalid board width" << endl;
//                return print_help();
//            }
//        }
//        else if( string(argv[i]) == "-h" )
//        {
//            if( sscanf(argv[++i], "%d", &boardSize.height) != 1 || boardSize.height <= 0 )
//            {
//                cout << "invalid board height" << endl;
//                return print_help();
//            }
//        }
//        else if( string(argv[i]) == "-nr" )
//            showRectified = false;
//        else if( string(argv[i]) == "--help" )
//            return print_help();
//        else if( argv[i][0] == '-' )
//        {
//            cout << "invalid option " << argv[i] << endl;
//            return 0;
//        }
//        else
//            imagelistfn = argv[i];
//    }
//
//    if( imagelistfn == "" )
//    {
//        imagelistfn = "../data/stereo_calib.xml";
//        boardSize = Size(9, 6);
//    }
//    else if( boardSize.width <= 0 || boardSize.height <= 0 )
//    {
//        cout << "if you specified XML file with chessboards, you should also specify the board width and height (-w and -h options)" << endl;
//        return 0;
//    }
//
//    vector<string> imagelist;
//    bool ok = readStringList(imagelistfn, imagelist);
//    if(!ok || imagelist.empty())
//    {
//        cout << "can not open " << imagelistfn << " or the string list is empty" << endl;
//        return print_help();
//    }
//
//    StereoCalib(imagelist, boardSize, true, showRectified);
//    return 0;
//}



















//stereo_match.cpp 
///*
// *  stereo_match.cpp
// *  calibration
// *
// *  Created by Victor  Eruhimov on 1/18/10.
// *  Copyright 2010 Argus Corp. All rights reserved.
// *
// */
//
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/core/utility.hpp"
//
//#include <stdio.h>
//
//using namespace cv;
//
//static void print_help()
//{
//    printf("\nDemo stereo matching converting L and R images into disparity and point clouds\n");
//    printf("\nUsage: stereo_match <left_image> <right_image> [--algorithm=bm|sgbm|hh] [--blocksize=<block_size>]\n"
//           "[--max-disparity=<max_disparity>] [--scale=scale_factor>] [-i <intrinsic_filename>] [-e <extrinsic_filename>]\n"
//           "[--no-display] [-o <disparity_image>] [-p <point_cloud_file>]\n");
//}
//
//static void saveXYZ(const char* filename, const Mat& mat)
//{
//    const double max_z = 1.0e4;
//    FILE* fp = fopen(filename, "wt");
//    for(int y = 0; y < mat.rows; y++)
//    {
//        for(int x = 0; x < mat.cols; x++)
//        {
//            Vec3f point = mat.at<Vec3f>(y, x);
//            if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
//            fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
//        }
//    }
//    fclose(fp);
//}
//
//int main(int argc, char** argv)
//{
//    const char* algorithm_opt = "--algorithm=";
//    const char* maxdisp_opt = "--max-disparity=";
//    const char* blocksize_opt = "--blocksize=";
//    const char* nodisplay_opt = "--no-display";
//    const char* scale_opt = "--scale=";
//
//    if(argc < 3)
//    {
//        print_help();
//        return 0;
//    }
//    const char* img1_filename = 0;
//    const char* img2_filename = 0;
//    const char* intrinsic_filename = 0;
//    const char* extrinsic_filename = 0;
//    const char* disparity_filename = 0;
//    const char* point_cloud_filename = 0;
//
//    enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3 };
//    int alg = STEREO_SGBM;
//    int SADWindowSize = 0, numberOfDisparities = 0;
//    bool no_display = false;
//    float scale = 1.f;
//
//    Ptr<StereoBM> bm = StereoBM::create(16,9);
//    Ptr<StereoSGBM> sgbm = StereoSGBM::create(0,16,3);
//
//    for( int i = 1; i < argc; i++ )
//    {
//        if( argv[i][0] != '-' )
//        {
//            if( !img1_filename )
//                img1_filename = argv[i];
//            else
//                img2_filename = argv[i];
//        }
//        else if( strncmp(argv[i], algorithm_opt, strlen(algorithm_opt)) == 0 )
//        {
//            char* _alg = argv[i] + strlen(algorithm_opt);
//            alg = strcmp(_alg, "bm") == 0 ? STEREO_BM :
//                  strcmp(_alg, "sgbm") == 0 ? STEREO_SGBM :
//                  strcmp(_alg, "hh") == 0 ? STEREO_HH :
//                  strcmp(_alg, "var") == 0 ? STEREO_VAR : -1;
//            if( alg < 0 )
//            {
//                printf("Command-line parameter error: Unknown stereo algorithm\n\n");
//                print_help();
//                return -1;
//            }
//        }
//        else if( strncmp(argv[i], maxdisp_opt, strlen(maxdisp_opt)) == 0 )
//        {
//            if( sscanf( argv[i] + strlen(maxdisp_opt), "%d", &numberOfDisparities ) != 1 ||
//                numberOfDisparities < 1 || numberOfDisparities % 16 != 0 )
//            {
//                printf("Command-line parameter error: The max disparity (--maxdisparity=<...>) must be a positive integer divisible by 16\n");
//                print_help();
//                return -1;
//            }
//        }
//        else if( strncmp(argv[i], blocksize_opt, strlen(blocksize_opt)) == 0 )
//        {
//            if( sscanf( argv[i] + strlen(blocksize_opt), "%d", &SADWindowSize ) != 1 ||
//                SADWindowSize < 1 || SADWindowSize % 2 != 1 )
//            {
//                printf("Command-line parameter error: The block size (--blocksize=<...>) must be a positive odd number\n");
//                return -1;
//            }
//        }
//        else if( strncmp(argv[i], scale_opt, strlen(scale_opt)) == 0 )
//        {
//            if( sscanf( argv[i] + strlen(scale_opt), "%f", &scale ) != 1 || scale < 0 )
//            {
//                printf("Command-line parameter error: The scale factor (--scale=<...>) must be a positive floating-point number\n");
//                return -1;
//            }
//        }
//        else if( strcmp(argv[i], nodisplay_opt) == 0 )
//            no_display = true;
//        else if( strcmp(argv[i], "-i" ) == 0 )
//            intrinsic_filename = argv[++i];
//        else if( strcmp(argv[i], "-e" ) == 0 )
//            extrinsic_filename = argv[++i];
//        else if( strcmp(argv[i], "-o" ) == 0 )
//            disparity_filename = argv[++i];
//        else if( strcmp(argv[i], "-p" ) == 0 )
//            point_cloud_filename = argv[++i];
//        else
//        {
//            printf("Command-line parameter error: unknown option %s\n", argv[i]);
//            return -1;
//        }
//    }
//
//    if( !img1_filename || !img2_filename )
//    {
//        printf("Command-line parameter error: both left and right images must be specified\n");
//        return -1;
//    }
//
//    if( (intrinsic_filename != 0) ^ (extrinsic_filename != 0) )
//    {
//        printf("Command-line parameter error: either both intrinsic and extrinsic parameters must be specified, or none of them (when the stereo pair is already rectified)\n");
//        return -1;
//    }
//
//    if( extrinsic_filename == 0 && point_cloud_filename )
//    {
//        printf("Command-line parameter error: extrinsic and intrinsic parameters must be specified to compute the point cloud\n");
//        return -1;
//    }
//
//    int color_mode = alg == STEREO_BM ? 0 : -1;
//    Mat img1 = imread(img1_filename, color_mode);
//    Mat img2 = imread(img2_filename, color_mode);
//
//    if (img1.empty())
//    {
//        printf("Command-line parameter error: could not load the first input image file\n");
//        return -1;
//    }
//    if (img2.empty())
//    {
//        printf("Command-line parameter error: could not load the second input image file\n");
//        return -1;
//    }
//
//    if (scale != 1.f)
//    {
//        Mat temp1, temp2;
//        int method = scale < 1 ? INTER_AREA : INTER_CUBIC;
//        resize(img1, temp1, Size(), scale, scale, method);
//        img1 = temp1;
//        resize(img2, temp2, Size(), scale, scale, method);
//        img2 = temp2;
//    }
//
//    Size img_size = img1.size();
//
//    Rect roi1, roi2;
//    Mat Q;
//
//    if( intrinsic_filename )
//    {
//        // reading intrinsic parameters
//        FileStorage fs(intrinsic_filename, FileStorage::READ);
//        if(!fs.isOpened())
//        {
//            printf("Failed to open file %s\n", intrinsic_filename);
//            return -1;
//        }
//
//        Mat M1, D1, M2, D2;
//        fs["M1"] >> M1;
//        fs["D1"] >> D1;
//        fs["M2"] >> M2;
//        fs["D2"] >> D2;
//
//        M1 *= scale;
//        M2 *= scale;
//
//        fs.open(extrinsic_filename, FileStorage::READ);
//        if(!fs.isOpened())
//        {
//            printf("Failed to open file %s\n", extrinsic_filename);
//            return -1;
//        }
//
//        Mat R, T, R1, P1, R2, P2;
//        fs["R"] >> R;
//        fs["T"] >> T;
//
//        stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );
//
//        Mat map11, map12, map21, map22;
//        initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
//        initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);
//
//        Mat img1r, img2r;
//        remap(img1, img1r, map11, map12, INTER_LINEAR);
//        remap(img2, img2r, map21, map22, INTER_LINEAR);
//
//        img1 = img1r;
//        img2 = img2r;
//    }
//
//    numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((img_size.width/8) + 15) & -16;
//
//    bm->setROI1(roi1);
//    bm->setROI2(roi2);
//    bm->setPreFilterCap(31);
//    bm->setBlockSize(SADWindowSize > 0 ? SADWindowSize : 9);
//    bm->setMinDisparity(0);
//    bm->setNumDisparities(numberOfDisparities);
//    bm->setTextureThreshold(10);
//    bm->setUniquenessRatio(15);
//    bm->setSpeckleWindowSize(100);
//    bm->setSpeckleRange(32);
//    bm->setDisp12MaxDiff(1);
//
//    sgbm->setPreFilterCap(63);
//    int sgbmWinSize = SADWindowSize > 0 ? SADWindowSize : 3;
//    sgbm->setBlockSize(sgbmWinSize);
//
//    int cn = img1.channels();
//
//    sgbm->setP1(8*cn*sgbmWinSize*sgbmWinSize);
//    sgbm->setP2(32*cn*sgbmWinSize*sgbmWinSize);
//    sgbm->setMinDisparity(0);
//    sgbm->setNumDisparities(numberOfDisparities);
//    sgbm->setUniquenessRatio(10);
//    sgbm->setSpeckleWindowSize(100);
//    sgbm->setSpeckleRange(32);
//    sgbm->setDisp12MaxDiff(1);
//    sgbm->setMode(alg == STEREO_HH ? StereoSGBM::MODE_HH : StereoSGBM::MODE_SGBM);
//
//    Mat disp, disp8;
//    //Mat img1p, img2p, dispp;
//    //copyMakeBorder(img1, img1p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
//    //copyMakeBorder(img2, img2p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
//
//    int64 t = getTickCount();
//    if( alg == STEREO_BM )
//        bm->compute(img1, img2, disp);
//    else if( alg == STEREO_SGBM || alg == STEREO_HH )
//        sgbm->compute(img1, img2, disp);
//    t = getTickCount() - t;
//    printf("Time elapsed: %fms\n", t*1000/getTickFrequency());
//
//    //disp = dispp.colRange(numberOfDisparities, img1p.cols);
//    if( alg != STEREO_VAR )
//        disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
//    else
//        disp.convertTo(disp8, CV_8U);
//    if( !no_display )
//    {
//        namedWindow("left", 1);
//        imshow("left", img1);
//        namedWindow("right", 1);
//        imshow("right", img2);
//        namedWindow("disparity", 0);
//        imshow("disparity", disp8);
//        printf("press any key to continue...");
//        fflush(stdout);
//        waitKey();
//        printf("\n");
//    }
//
//    if(disparity_filename)
//        imwrite(disparity_filename, disp8);
//
//    if(point_cloud_filename)
//    {
//        printf("storing the point cloud...");
//        fflush(stdout);
//        Mat xyz;
//        reprojectImageTo3D(disp, xyz, Q, true);
//        saveXYZ(point_cloud_filename, xyz);
//        printf("\n");
//    }
//
//    return 0;
//}
