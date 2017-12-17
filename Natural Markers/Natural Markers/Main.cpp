#include <iostream>
#include <string>
#include "Preparation.h"
#include <direct.h>

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

int main(int argc, char** argv)
{
	int opt;
	string imagePath;
	string savePath;
	Preparation p;
	do
	{
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
			cout << "Augmentation" << endl;
			break;
		default:
			break;
		}
	} while (opt!=3);
	
	system("pause");
	return 0;
}