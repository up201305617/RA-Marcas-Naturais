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

//Get full path of folder
string getFullPath(string folder)
{
	char * path = NULL;
	path = _getcwd(NULL, 0);
	string fullPath = path + folder;
	return fullPath;
}

//Create directory with the name "folder"
string createDirectory(string folder)
{
	string fullPath = getFullPath(folder);
	const char * p_path = fullPath.c_str();
	_mkdir(p_path);
	return fullPath;
}

//Get all files from path
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

//Convert user input into detector name
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

//Convert user input into extractor name
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

//Convert user input into matcher name
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

//Convert user input into test mode boolean
bool getMode(int opt)
{
	if (opt == 1)
	{
		return true;
	}
	else if (opt == 2)
	{
		return false;
	}
}

//Build the path to a file in database 
string buildPathToDatabase(string path, string name)
{
	string full = path + "\\" + name;
	return full;
}

//Build the path to all files in database
vector<string> buildPathToImage(vector<string> names)
{
	vector<string> paths;
	for (int i = 0; i < names.size(); i++)
	{
		string path = buildPathToDatabase(getFullPath("\\database"), names[i]);
		paths.push_back(path);
	}
	return paths;
}

//Delete all files in database
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
	int testOpt;
	string imagePath;
	string savePath;
	string scenePath;
	Preparation p;
	Augmentation a;
	vector<string> names;
	vector<string> paths;

	do
	{
		system("cls");
		cout << "================================================================" << endl;
		cout << "=========== Augmented Reality with Natural Markers =============" << endl;
		cout << "================================================================" << endl << endl;
		cout << "Choose one of the following mods:" << endl << endl;
		cout << "1. Preparation" << endl;
		cout << "2. Augmentation" << endl;
		cout << "3. Reset Database" << endl;
		cout << "4. Exit" << endl << endl;
		cout << "Mode: ";
		cin >> opt;
		switch (opt)
		{
		case 1:
			system("cls");
			savePath = createDirectory("\\database");
			cout << "=============================================" << endl;
			cout << "============= Preparation Mode: =============" << endl;
			cout << "=============================================" << endl << endl;
			cout << "Path to Image: ";
			cin >> imagePath;
			p = Preparation(imagePath, savePath);
			p.init();
			break;
		case 2:
			system("cls");
			names = getDirectoryFiles(getFullPath("\\database"));
			paths = buildPathToImage(names);
			cout << "=============================================" << endl;
			cout << "============= Augmentation Mode =============" << endl;
			cout << "=============================================" << endl;
			cout << endl << "Possible Combinations" << endl << endl;
			cout << "DETECTOR " << "EXTRACTOR " << "MATCHER" << endl;
			cout << "SURF     " << "SURF      " << "FLANN" << endl;
			cout << "SURF     " << "FREAK     " << "BFM" << endl;
			cout << "SURF     " << "SURF      " << "BFM" << endl;
			cout << "SIFT     " << "SIFT      " << "FLANN" << endl;
			cout << "SIFT     " << "SURF      " << "FLANN" << endl;
			cout << "SIFT     " << "SURF      " << "BFM" << endl;
			cout << "FAST     " << "SURF      " << "FLANN" << endl;
			cout << "FAST     " << "SIFT      " << "FLANN" << endl;
			cout << "FAST     " << "BRIEF     " << "BFM" << endl;
			cout << "FAST     " << "FREAK     " << "BFM" << endl;
			cout << "FAST     " << "ORB       " << "BFM" << endl;
			cout << "ORB      " << "ORB       " << "BFM" << endl;
			cout << "ORB      " << "BRIEF     " << "BFM" << endl << endl;
			cout << "Feature Detector" << endl;
			cout << "1. FAST" << endl << "2. SIFT" << endl << "3. SURF" << endl << "4. ORB" << endl;
			
			do
			{
				cout << "Detector: ";
				cin >> detectorOpt;
			} while (detectorOpt > 4 || detectorOpt <= 0);
			system("cls");
			cout << "Descriptor Extractor" << endl;
			cout << "1. SIFT" << endl << "2. SURF" << endl << "3. ORB" << endl << "4. BRIEF" << endl << "5. FREAK" << endl;
			
			do
			{
				cout << "Extractor: ";
				cin >> extractorOpt;
			} while (extractorOpt > 5 || extractorOpt <= 0);
			system("cls");
			cout << "Descriptor Matcher" << endl;
			cout << "1. FLANN" << endl << "2. BFM" << endl;
			
			do
			{
				cout << "Matcher: ";
				cin >> matcherOpt;
			} while (matcherOpt > 2 || matcherOpt <= 0);
			system("cls");
			cout << "Test Mode?" << endl;
			cout << "1. Yes" << endl << "2. No" << endl;
			
			do
			{
				cout << "Mode: ";
				cin >> testOpt;
			} while (testOpt > 2 || testOpt <= 0);
			system("cls");
			cout << "Path to Image: ";
			cin >> scenePath;
			a = Augmentation(scenePath, names, getDetector(detectorOpt), getExtractor(extractorOpt), getMatcher(matcherOpt), paths, getMode(testOpt));
			a.init();
			system("pause");
			break;
		case 3:
			names = getDirectoryFiles(getFullPath("\\database"));
			paths = buildPathToImage(names);
			resetDatabase(paths);
			system("pause");
			break;
		default:
			break;
		}
	} while (opt != 4 || opt > 4 || opt < 0);
	
	system("pause");
	return 0;
}