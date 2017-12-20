#include "Augmentation.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

//Default constructor
Augmentation::Augmentation() {};

//Destructor
Augmentation::~Augmentation() {};

//Constructor
Augmentation::Augmentation(string i, vector<string> im, string d, string e, string m, vector<string> p, bool t)
{
	this->imagePath = i;
	this->imgNames = im;
	this->paths = p;
	this->testMode = t;

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

//Matching descriptors and return the best ones
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

//Open image in color
bool Augmentation::openImageAugmentation(const std::string &filename, Mat &image)
{
	image = imread(filename);

	if (!image.data)
	{
		cout << endl << "Error reading image " << filename << endl;
		return false;
	}

	return true;
}

//Draw the region of interest on img
void Augmentation::draw(Mat img, vector <Point2f> scene_corners)
{
	line(img, scene_corners[0], scene_corners[1], Scalar(255, 0, 0), 4);
	line(img, scene_corners[1], scene_corners[2], Scalar(255, 0, 0), 4);
	line(img, scene_corners[2], scene_corners[3], Scalar(255, 0, 0), 4);
	line(img, scene_corners[3], scene_corners[0], Scalar(255, 0, 0), 4);
}

//Mode that shows all steps
void Augmentation::debugMode(Mat db_image, vector<KeyPoint> db_keypoints, Mat scene, vector<KeyPoint> scene_key, Mat descriptors_database, Mat descriptors_scene, int i)
{
	Mat db_output, scene_output, all_matches;
	vector<DMatch> matches;

	//Draw keypoints of scene image
	cout << "Detect keypoints of scene image." << endl;
	drawKeypoints(scene, scene_key, scene_output, Scalar::all(-1));
	imshow("Scene", scene_output);

	//Draw keypoints of database image
	cout << "Detect keypoints of database image: " << this->imgNames[i] << endl;
	drawKeypoints(db_image, db_keypoints, db_output, Scalar::all(-1));
	imshow("Keypoints of "+this->imgNames[i], db_output);
	waitKey(1);

	//Draw all matches
	cout << "Matching all keypoints between scene and " << this->imgNames[i] << endl;
	this->matcher->match(descriptors_database, descriptors_scene, matches);
	drawMatches(db_image, db_keypoints, scene, scene_key, matches, all_matches, Scalar::all(-1), CV_RGB(255, 255, 255), Mat(), 2);
	imshow("All Matches of "+this->imgNames[i], all_matches);
	waitKey(1);
}

//Check if inliners are inside the corners of the possible region of interest
bool Augmentation::checkInliers(vector<Point2f> inlier_points, vector<Point2f> corners)
{
	for (unsigned int i = 0; i < inlier_points.size(); ++i)
	{
		if (pointPolygonTest(corners, inlier_points[i], true) < 0)
		{
			return false;
		}
	}
	return true;
}

//Init the augmentation program
int Augmentation::init()
{
	Mat scene, scene_gray, result;
	Mat database;
	vector<KeyPoint> keypoints_database, keypoints_scene;
	Mat descriptors_database, descriptors_scene;

	//Open scene image
	if (!openImageAugmentation(this->imagePath, scene))
	{
		cout << "Could not open image!" << endl;
		return -1;
	}

	//Convert to grayscale
	cvtColor(scene, scene_gray, CV_BGR2GRAY);

	//Analyse all images on database
	for (int i = 0; i < this->paths.size(); i++)
	{
		//Open database image
		if (!openImageAugmentation(this->paths[i], database))
		{
			cout << "Could not open image!" << endl;
			return -1;
		}

		//Convert to grayscale
		cvtColor(database, database, CV_BGR2GRAY);

		//Detect keypoints
		this->detector->detect(scene_gray, keypoints_scene);
		this->detector->detect(database, keypoints_database);

		//Extract descriptors
		this->extractor->compute(scene_gray, keypoints_scene, descriptors_scene);
		this->extractor->compute(database, keypoints_database, descriptors_database);

		//Debug Mode
		if (this->testMode)
		{
			debugMode(database, keypoints_database, scene_gray, keypoints_scene, descriptors_database, descriptors_scene,i);
		}

		//Matching descriptors
		vector<DMatch> good_matches = getGoodMatches(descriptors_database, descriptors_scene);

		if (good_matches.size() < 4)
		{
			cout << "Does not have enough points." << endl;
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
					inlier_points.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
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
			
			if (this->testMode)
			{
				drawMatches(database, keypoints_database, scene_gray, keypoints_scene, inliers, result, Scalar::all(-1), CV_RGB(255, 255, 255), Mat(), 2);

				line(result, scene_corners[0] + Point2f(database.cols, 0), scene_corners[1] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
				line(result, scene_corners[1] + Point2f(database.cols, 0), scene_corners[2] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
				line(result, scene_corners[2] + Point2f(database.cols, 0), scene_corners[3] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);
				line(result, scene_corners[3] + Point2f(database.cols, 0), scene_corners[0] + Point2f(database.cols, 0), Scalar(0, 255, 0), 4);

				imshow(this->imgNames[i], result);
				waitKey(1);
			}

			//Draw rectangle on scene, only if all inliners are in scene_corners
			if (checkInliers(inlier_points, scene_corners))
			{
				draw(scene, scene_corners);
			}
		}
	}

	imshow("Final", scene);
	waitKey(0);
	destroyAllWindows();

	return 0;
}