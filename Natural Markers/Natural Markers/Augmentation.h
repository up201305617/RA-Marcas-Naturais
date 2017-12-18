#pragma once
#include "opencv2\core\core.hpp"
#include "opencv2\features2d\features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

class Augmentation
{
private:
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
	DescriptorMatcher *matcher;
	string imagePath;
public:
	Augmentation();
	Augmentation(string i, string d, string e, string m);
	~Augmentation();
	int init();
	vector< DMatch > get_good_matches(Mat descriptors_database, Mat descriptors_scene);
};