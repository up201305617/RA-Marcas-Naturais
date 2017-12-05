#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <windows.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

FlannBasedMatcher *matcher = new FlannBasedMatcher();
//BFMatcher *bMatcher = new BFMatcher(NORM_L2, false);

int readDatabase(vector<string> &filenames, const string &directory)
{
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	do
	{
		string file_name = file_data.cFileName;
		string full_file_name = directory + "/" + file_name;
		bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
		{
			continue;
		}

		if (is_directory)
		{
			continue;
		}

		filenames.push_back(full_file_name);
	} 
	while (FindNextFile(dir, &file_data));

	FindClose(dir);

	return(filenames.size());
}

/*void calculateMinMaxDistance(Mat descriptor, vector<DMatch> matches)
{
	for (int i = 0; i < descriptor.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist)
			min_dist = dist;

		if (dist > max_dist)
			max_dist = dist;
	}
}

vector<DMatch> getBetterMatches(Mat descriptor, vector<DMatch> matches)
{
	vector<DMatch> better_matches;
	
	for (int i = 0; i < descriptor.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			better_matches.push_back(matches[i]);
		}
	}

	return better_matches;
}*/

vector<DMatch> getGoodMatches(Mat descriptors_obj, Mat scene_descriptors)
{
	vector<DMatch> matches;
	vector<DMatch> good_matches;
	
	double max_dist = 0;
	double min_dist = 100;
	
	matcher->match(descriptors_obj, scene_descriptors, matches);

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

int main(int argc, char** argv)
{
	vector<string> filenames;
	
	Mat image_database = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat image = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);

	//Detect keypoints
	Ptr<SIFT> detector = SIFT::create();
	vector<KeyPoint> keypoints_database, keypoints_image;
	Mat descriptors_database, descriptors_image;
	detector->detectAndCompute(image_database, Mat(), keypoints_database, descriptors_database);
	detector->detectAndCompute(image, Mat(), keypoints_image, descriptors_image);

	//Matcher
	//vector<DMatch> matches;
	//matcher.match(descriptors_database, descriptors_image, matches);
	
	//calculateMinMaxDistance(descriptors_database, matches);
	//vector<DMatch> better_matches = getBetterMatches(descriptors_database, matches);

	vector<DMatch> good_matches = getGoodMatches(descriptors_database, descriptors_image);

	Mat img_matches;
	drawMatches(image_database, keypoints_database, image, keypoints_image,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	namedWindow("Matches", 1);
	imshow("Matches", img_matches);

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(keypoints_database[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_image[good_matches[i].trainIdx].pt);
	}

	Mat mask;
	Mat homography = findHomography(obj, scene, RANSAC, 3, mask);

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); 
	obj_corners[1] = cvPoint(image_database.cols, 0);
	obj_corners[2] = cvPoint(image_database.cols, image_database.rows);
	obj_corners[3] = cvPoint(0, image_database.rows);

	std::vector<Point2f> scene_corners(4);
	
	perspectiveTransform(obj_corners, scene_corners, homography);

	line(img_matches, scene_corners[0] + Point2f(image_database.cols, 0), scene_corners[1] + Point2f(image_database.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(image_database.cols, 0), scene_corners[2] + Point2f(image_database.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(image_database.cols, 0), scene_corners[3] + Point2f(image_database.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(image_database.cols, 0), scene_corners[0] + Point2f(image_database.cols, 0), Scalar(0, 255, 0), 4);

	Mat color = imread(argv[2]);

	line(color, scene_corners[0], scene_corners[1], Scalar(255, 0, 0), 4);
	line(color, scene_corners[1], scene_corners[2], Scalar(255, 0, 0), 4);
	line(color, scene_corners[2], scene_corners[3], Scalar(255, 0, 0), 4);
	line(color, scene_corners[3], scene_corners[0], Scalar(255, 0, 0), 4);

	namedWindow("Final", 1);
	imshow("Final", img_matches);

	namedWindow("Original", 1);
	imshow("Original", color);

	/*FileStorage fs2("Keypoints.yml", FileStorage::READ);
	FileNode kptFileNode = fs2["code"];
	read(kptFileNode, keypoints_database);
	fs2.release();*/

	waitKey(0);
	return 0;
}