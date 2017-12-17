#pragma once
#include <string>

class Preparation
{
private:
	std::string image;
	std::string savePath;
public:
	Preparation(std::string i, std::string p);
	Preparation();
	~Preparation(void);
	int init();
};