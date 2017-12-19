#include "Augmentation.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

Augmentation::Augmentation() {};

Augmentation::~Augmentation() {};

Augmentation::Augmentation(string i, string o, string d, string e, string m)
{
	this->imagePath = i;
	this->objPath = o;
	
	if (m == "FLANN")
	{
		this->matcher = new FlannBasedMatcher();
	}
	else if (m == "BFM")
	{
		this->matcher = new BFMatcher(NORM_HAMMING, true);
	}

	if (d == "FAST")
	{
		this->detector = FastFeatureDetector::create();
	}
	else if(d == "SIFT")
	{
		this->detector = SiftFeatureDetector::create();
	}
	else if (d == "SURF")
	{
		this->detector = SurfFeatureDetector::create();
	}
	else if (d == "ORB")
	{
		this->detector = ORB::create();
	}

	if (e == "SIFT")
	{
		this->extractor = SiftDescriptorExtractor::create();

		if (m == "BFM")
		{
			this->matcher = new BFMatcher(NORM_L2, false);
		}
	}
	else if(e == "SURF")
	{
		this->extractor = SurfDescriptorExtractor::create();
		
		if (m == "BFM")
		{
			this->matcher = new BFMatcher(NORM_L2, false);
		}
	}
	else if (e == "ORB")
	{
		this->extractor = ORB::create();
	}
	else if(e == "BRIEF")
	{
		this->extractor = BriefDescriptorExtractor::create();
	}
	else if (e == "FREAK")
	{
		this->extractor = FREAK::create();
	}
}

vector<DMatch> Augmentation::getGoodMatches(Mat descriptors_database, Mat descriptors_scene)
{
	vector<DMatch> matches;
	vector<DMatch> good_matches;

	double max_dist = 0;
	double min_dist = 100;

	this->matcher->match(descriptors_database, descriptors_scene, matches);

	for (unsigned i = 0; i < matches.size(); i++)
	{
		double dist = matches[i].distance;

		if (dist < min_dist)
			min_dist = dist;

		if (dist > max_dist)
			max_dist = dist;
	}

	for (unsigned i = 0; i < matches.size(); i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	return good_matches;
}

bool openImageAugmentation(const std::string &filename, Mat &image)
{
	image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

	if (!image.data)
	{
		cout << endl << "Error reading image " << filename << endl;
		return false;
	}

	return true;
}

void Augmentation::draw(Mat img, vector<vector <Point2f>> scene_corners)
{
	for (int i = 0; i < scene_corners.size(); i++)
	{
		line(img, scene_corners[i][0], scene_corners[i][1], Scalar(255, 0, 0), 4);
		line(img, scene_corners[i][1], scene_corners[i][2], Scalar(255, 0, 0), 4);
		line(img, scene_corners[i][2], scene_corners[i][3], Scalar(255, 0, 0), 4);
		line(img, scene_corners[i][3], scene_corners[i][0], Scalar(255, 0, 0), 4);
	}	
}

int Augmentation::init()
{
	Mat scene;
	Mat database;
	vector<KeyPoint> keypoints_database, keypoints_scene;
	Mat descriptors_database, descriptors_scene;

	//Open scene image
	if (!openImageAugmentation(this->imagePath, scene))
	{
		cout << "Could not open image!" << endl;
		return -1;
	}

	//Open database image
	if (!openImageAugmentation(this->objPath, database))
	{
		cout << "Could not open image!" << endl;
		return -1;
	}

	//Detect keypoints
	this->detector->detect(scene, keypoints_scene);
	this->detector->detect(database, keypoints_database);

	//Extract descriptors
	this->extractor->compute(scene, keypoints_scene, descriptors_scene);
	this->extractor->compute(database, keypoints_database, descriptors_database);

	//Matching descriptors
	vector<DMatch> good_matches = getGoodMatches(descriptors_database, descriptors_scene);

	if (good_matches.size() < 4)
	{
		cout << "Does not have enough points." << endl;
		return -1;
	}
	else
	{
		//Prepare data to findHomography
		vector<Point2f> database_points;
		vector<Point2f> scene_points;

		for (size_t i = 0; i < good_matches.size(); i++)
		{
			database_points.push_back(keypoints_database[good_matches[i].queryIdx].pt);
			scene_points.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
		}

		// Find homography matrix and get inliers mask    
		vector<unsigned char> inliersMask;
		Mat homography = findHomography(database_points, scene_points, RANSAC, 3, inliersMask);
		vector<DMatch> inliers;
		vector<Point2f> inlier_points;

		for (size_t i = 0; i<inliersMask.size(); i++)
		{
			if (inliersMask[i])
			{
				inliers.push_back(good_matches[i]);
				inlier_points.push_back(keypoints_scene[good_matches[i].queryIdx].pt);
			}
		}

		//Apply homography to the object corners position to match the one in the scene
		std::vector<Point2f> obj_corners(4);
		std::vector<Point2f> scene_corners(4);

		obj_corners[0] = cvPoint(0, 0);
		obj_corners[1] = cvPoint(database.cols, 0);
		obj_corners[2] = cvPoint(database.cols, database.rows);
		obj_corners[3] = cvPoint(0, database.rows);

		perspectiveTransform(obj_corners, scene_corners, homography);

		//Draw results
		Mat result;
		drawMatches(database, keypoints_database, scene, keypoints_scene, good_matches, result, Scalar::all(-1), CV_RGB(255, 255, 255), Mat(), 2);

		line(result, scene_corners[0] + Point2f(database.cols, 0), scene_corners[1] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
		line(result, scene_corners[1] + Point2f(database.cols, 0), scene_corners[2] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
		line(result, scene_corners[2] + Point2f(database.cols, 0), scene_corners[3] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
		line(result, scene_corners[3] + Point2f(database.cols, 0), scene_corners[0] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
		
		imshow("result", result);
		waitKey(0);
	}

	return 0;
}