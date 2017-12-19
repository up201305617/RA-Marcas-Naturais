#include "Preparation.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <direct.h>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

Preparation::Preparation(std::string i, std::string p)
{
	this->image = i;
	this->savePath = p;
}

Preparation::Preparation() {}

Preparation::~Preparation(void) {}

bool openImage(const std::string &filename, Mat &image)
{
	image = imread(filename);

	if (!image.data)
	{
		cout << endl << "Error reading image " << filename << endl;
		return false;
	}

	return true;
}

int Preparation::init()
{
	Mat image;
	vector< Rect > rects;
	
	if (!openImage(this->image, image))
	{
		cout << "Could not open image!" << endl;
		return -1;
	}

	selectROIs("Preparation", image, rects, false, false);
	
	destroyWindow("Preparation");

	for (int i = 0; i < rects.size(); i++)
	{
		Mat roi = image(rects[i]);
		stringstream ss;
		cout << "Name the ROI number " << i+1 << endl;
		string name;
		cin >> name;
		ss << this->savePath << "\\" << name << ".jpg";
		imwrite(ss.str(), roi);
	}
	
	waitKey(0);
	return 0;
}