#include <iostream>
#include <string>
#include "Preparation.h"
#include "Augmentation.h"
#include <direct.h>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <stdio.h>

using namespace std;

string getFullPath(string folder)
{
	char * path = NULL;
	path = _getcwd(NULL, 0);
	string fullPath = path + folder;
	return fullPath;
}

string createDirectory(string folder)
{
	string fullPath = getFullPath(folder);
	const char * p_path = fullPath.c_str();
	_mkdir(p_path);
	return fullPath;
}

vector<string> getDirectoryFiles(string path)
{
	string pattern(path);
	vector<string> temps;
	vector<string> names;
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;

	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			temps.push_back(data.cFileName);
	
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}

	for (int i = 0; i < temps.size(); i++)
	{
		if (temps[i] != ".")
		{
			if(temps[i] != "..")
				names.push_back(temps[i]);
		}
	}

	return names;
}

string getDetector(int opt)
{
	if (opt == 1)
	{
		return "FAST";
	}
	else if (opt == 2)
	{
		return "SIFT";
	}
	else if (opt == 3)
	{
		return "SURF";
	}
	else if (opt == 4)
	{
		return "ORB";
	}
}

string getExtractor(int opt)
{
	if (opt == 1)
	{
		return "SIFT";
	}
	else if (opt == 2)
	{
		return "SURF";
	}
	else if (opt == 3)
	{
		return "ORB";
	}
	else if (opt == 4)
	{
		return "BRIEF";
	}
	else if (opt == 5)
	{
		return "FREAK";
	}
}

string getMatcher(int opt)
{
	if (opt == 1)
	{
		return "FLANN";
	}
	else if (opt == 2)
	{
		return "BFM";
	}
}

string buildPathToDatabase(string path, string name)
{
	string full = path + "\\" + name;
	return full;
}

vector<string> buildPathToImage(vector<string> names)
{
	vector<string> paths;
	for (int i = 0; i < names.size(); i++)
	{
		string path = buildPathToDatabase(getFullPath("\\database"), names[i]);
		cout << path << endl;
		paths.push_back(path);
	}
	return paths;
}

void resetDatabase(vector<string> files)
{
	for (int i = 0; i < files.size(); i++)
	{
		remove(files[i].c_str());
	}
}

int main(int argc, char** argv)
{
	int opt;
	int detectorOpt;
	int extractorOpt;
	int matcherOpt;
	string imagePath;
	string savePath;
	string scenePath;
	Preparation p;
	Augmentation a;
	vector<string> names;

	do
	{
		system("cls");
		cout << "Choose one of the following mods:" << endl;
		cout << "1. Preparation" << endl;
		cout << "2. Augmentation" << endl;
		cout << "3. Reset Database" << endl;
		cout << "4. Exit" << endl;
		cout << "Mode: ";
		cin >> opt;
		switch (opt)
		{
		case 1:
			system("cls");
			savePath = createDirectory("\\database");
			cout << "Preparation Mode:" << endl;
			cout << "Path to Image: ";
			cin >> imagePath;
			cout << imagePath;
			p = Preparation(imagePath, savePath);
			p.init();
			break;
		case 2:
			system("cls");
			names = getDirectoryFiles(getFullPath("\\database"));
			buildPathToImage(names);
			cout << "Augmentation Mode" << endl;
			cout << "Feature Detector" << endl;
			cout << "1. FAST; 2. SIFT; 3. SURF; 4. ORB" << endl;
			do
			{
				cin >> detectorOpt;
			} while (detectorOpt > 4 || detectorOpt <= 0);
			cout << "Descriptor Extractor" << endl;
			cout << "1. SIFT; 2. SURF; 3. ORB; 4. BRIEF; 5. FREAK" << endl;
			do
			{
				cin >> extractorOpt;
			} while (extractorOpt > 5 || extractorOpt <= 0);
			cout << "Descriptor Matcher" << endl;
			cout << "1. FLANN; 2. BFM" << endl;
			do
			{
				cin >> matcherOpt;
			} while (matcherOpt > 2 || matcherOpt <= 0);
			cout << "Path to Image: ";
			cin >> scenePath;
			a = Augmentation(scenePath, buildPathToDatabase(getFullPath("\\database"),names[0]), getDetector(detectorOpt), getExtractor(extractorOpt), getMatcher(matcherOpt));
			a.init();
			system("pause");
			break;
		case 3:
			resetDatabase(buildPathToImage(names));
			break;
		default:
			break;
		}
	} while (opt != 4 || opt > 4 || opt < 0);
	
	system("pause");
	return 0;
}