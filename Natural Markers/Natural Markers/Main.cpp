#include <iostream>
#include <string>
#include "Preparation.h"
#include <direct.h>
#include <vector>
#include <conio.h>
#include <windows.h>

using namespace std;

string createDirectory(string folder)
{
	char * path = NULL;
	path = _getcwd(NULL, 0);
	string fullPath = path + folder;
	const char * p_path = fullPath.c_str();
	_mkdir(p_path);
	return fullPath;
}

string getFullPath(string folder)
{
	char * path = NULL;
	path = _getcwd(NULL, 0);
	string fullPath = path + folder;
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

int main(int argc, char** argv)
{
	int opt;
	string imagePath;
	string savePath;
	Preparation p;
	vector<string> names;

	do
	{
		system("cls");
		cout << "Choose one of the following mods:" << endl;
		cout << "1. Preparation" << endl;
		cout << "2. Augmentation" << endl;
		cout << "3. Exit" << endl;
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
			names = getDirectoryFiles(getFullPath("\\database"));
			cout << "Augmentation Mode" << endl;
			system("pause");
			break;
		default:
			break;
		}
	} while (opt!=3);
	
	system("pause");
	return 0;
}