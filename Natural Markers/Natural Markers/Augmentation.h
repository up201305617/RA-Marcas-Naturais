#pragma once
#include "opencv2\core\core.hpp"
#include "opencv2\features2d\features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace std;

class Augmentation
{
private:
	cv::Ptr<cv::FeatureDetector> detector;
	cv::Ptr<cv::DescriptorExtractor> extractor;
	cv::DescriptorMatcher *matcher;
	string imagePath;
	vector<string> imgNames;
	vector<string> paths;
	bool testMode;
public:
	Augmentation();
	Augmentation(string i, vector<string> im, string d, string e, string m, vector<string> p, bool t);
	~Augmentation();
	int init();
	vector< cv::DMatch > getGoodMatches(cv::Mat descriptors_database, cv::Mat descriptors_scene);
	void draw(cv::Mat img, vector<cv::Point2f> scene_corners, string name);
	void debugMode(cv::Mat db_image, vector<cv::KeyPoint> db_keypoints, cv::Mat scene, vector<cv::KeyPoint> scene_key, cv::Mat descriptors_database, cv::Mat descriptors_scene, int i);
	bool checkInliers(vector<cv::Point2f> inlier_points, vector<cv::Point2f> corners);
	bool openImageAugmentation(const std::string &filename, cv::Mat &image);
};