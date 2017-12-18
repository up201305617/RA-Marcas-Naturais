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
	string objPath;
public:
	Augmentation();
	Augmentation(string i, string o, string d, string e, string m);
	~Augmentation();
	int init();
	vector< cv::DMatch > getGoodMatches(cv::Mat descriptors_database, cv::Mat descriptors_scene);
};