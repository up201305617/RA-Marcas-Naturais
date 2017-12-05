#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

bool openImage(const std::string &filename, Mat &image)
{
	image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	
	if (!image.data) 
	{
		cout << "Error reading image " << filename << endl;
		return false;
	}

	return true;
}

int main(int argc, char** argv)
{
	Mat image;

	if (argc != 2)
	{
		cout << " Usage: <exe_name> <image_to_read>" << endl;
		return -1;
	}

	if (!openImage(argv[1], image))
	{
		return -1;
	}

	Rect2d r = selectROI(image,false,false);
	Mat crop = image(r);
	imwrite("crop.jpg", crop);
	namedWindow("Crop", 1);
	imshow("Crop", crop);

	Mat descriptors_1;

	Ptr<SURF> detector = SURF::create();
	std::vector<cv::KeyPoint> keypoints;
	std::vector<cv::KeyPoint> keypoints_1;
	detector->detect(image, keypoints);
	
	FileStorage fs("Keypoints.yml", FileStorage::WRITE);
	write(fs, "code",keypoints);
	fs.release();

	/*FileStorage fs2("Keypoints.yml", FileStorage::READ);
	FileNode kptFileNode = fs2["code"];
	read(kptFileNode, keypoints_1);
	fs2.release();*/

	Mat output;
	Mat testing;

	drawKeypoints(image, keypoints_1, testing);
	drawKeypoints(image, keypoints, output);
	namedWindow("Original", 1);
	imshow("Original", output);

	//namedWindow("Read", 1);
	//imshow("Read", testing);

	waitKey(0);
	return 0;
}