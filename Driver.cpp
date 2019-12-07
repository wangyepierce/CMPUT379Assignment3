#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cctype>
#include <locale>
#include <stdlib.h>
#include <bits/stdc++.h>
#include "FileSystem.h"
using namespace std;

string inputfile="in.txt";	//this is the input file in which all the commands are written
ifstream vDisk;	//this is filestream object which actually would be holding the virtual disk file
string fileName; //this is used for fileNames to create,delete, resize etc ..
int fileLength; //that's the length of the file to create *this will behave as block number as well
bool isMounted = false; //true it when you find a consistent file system and mount it on your file system.

// void trim(string& ref);

void mount(string str, int error_num)
{
	trim(str);
	if (str == "")
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() == 0)
	{
		const char *c = fileName.c_str();
		fs_mount(c);
	}
	else
		cerr << "Error: " << inputfile << ", " << error_num << endl;
}
void create(string str, int error_num)
{
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	if (fileName.length() > 5)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	string tempLength= str.substr(0, str.find(" "));
	int i = 0;
	while (i<tempLength.length())
	{
		if (tempLength[i] < '0' || tempLength[i] > '9')
		{
			cerr << "Error: " << inputfile << ", " << error_num << endl;
			return;
		}
		i++;
	}
	stringstream getInt(tempLength);
	getInt >> fileLength;
	/*if (fileLength == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}*/
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	const char *c = fileName.c_str();
	fs_create(c, fileLength);
}
void deleteIt(string str, int error_num)
{
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	if (fileName.length() > 5)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	const char *c = fileName.c_str();
	fs_delete(c);
}
void read(string str, int error_num)
{
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	if (fileName.length() > 5)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	string tempLength = str.substr(0, str.find(" "));
	int i = 0;
	while (i < tempLength.length())
	{
		if (tempLength[i] < '0' || tempLength[i] > '9')
		{
			cerr << "Error: " << inputfile << ", " << error_num << endl;
			return;
		}
		i++;
	}
	stringstream getInt(tempLength);
	getInt >> fileLength;
	/*if (fileLength == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}*/
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	const char *c = fileName.c_str();
	fs_read(c, fileLength);
}
void write(string str, int error_num)
{
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	if (fileName.length() > 5)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	string tempLength = str.substr(0, str.find(" "));
	int i = 0;
	while (i < tempLength.length())
	{
		if (tempLength[i] < '0' || tempLength[i] > '9')
		{
			cerr << "Error: " << inputfile << ", " << error_num << endl;
			return;
		}
		i++;
	}
	stringstream getInt(tempLength);
	getInt >> fileLength;
	/*if (fileLength == 0)
	{
		cout << 1;
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}*/
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	const char *c = fileName.c_str();
	fs_write(c, fileLength);
}
void buff(string str, int error_num)
{
	if (str == "")
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	const char *c = str.c_str();
	uint8_t buff[1024];
	int i = 0;
	while (c[i] != '\0')
	{
		buff[i] = c[i];
		i++;
	}
	while (i < 1024)
	{
		buff[i] = 0;
		i++;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	fs_buff(buff);
}
void list(string str, int error_num)
{
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	fs_ls();
}
void reSize(string str, int error_num)
{
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	if (fileName.length() > 5)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	string tempLength = str.substr(0, str.find(" "));
	int i = 0;
	while (i < tempLength.length())
	{
		if (tempLength[i] < '0' || tempLength[i] > '9')
		{
			cerr << "Error: " << inputfile << ", " << error_num << endl;
			return;
		}
		i++;
	}
	stringstream getInt(tempLength);
	getInt >> fileLength;
	/*if (fileLength == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}*/
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	const char *c = fileName.c_str();
	fs_resize(c, fileLength);
}
void defrag(string str, int error_num)
{
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	fs_defrag();
}
void cd(string str, int error_num)
{
	trim(str);
	if (str.length() == 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	fileName = str.substr(0, str.find(" "));
	if (fileName.length() > 5)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	str.erase(0, str.find(" "));
	trim(str);
	if (str.length() > 0)
	{
		cerr << "Error: " << inputfile << ", " << error_num << endl;
		return;
	}
	if (!isMounted)
	{
		cerr << "Error: No file system is mounted\n";
		return;
	}
	const char *c = fileName.c_str();
	fs_cd(c);
}
int main()
{
	
	cout << "Enter the input file: "; cin >> inputfile;
	vDisk.open(inputfile);
	string str;
	string command;
	int i = 1;
	while (!vDisk.eof())
	{
		getline(vDisk, str);
		command = str.substr(0, str.find(" "));
		str.erase(0, 2);
		if (command.length() > 1)
			cerr << "Error: " << inputfile << ", " << i << endl;
		else if (command == "M")
			mount(str, i);
		else if (command == "C")
			create(str, i);
		else if (command == "D")
			deleteIt(str, i);
		else if (command == "R")
			read(str, i);
		else if (command == "W")
			write(str, i);
		else if (command == "B")
			buff(str, i);
		else if (command == "L")
			list(str, i);
		else if (command == "E")
			reSize(str, i);
		else if (command == "O")
			defrag(str, i);
		else if (command == "Y")
			cd(str, i);
		else
			cerr << "Error: " << inputfile << ", " << i << endl;
		i++;
	}
	vDisk.close();
	return 0;
}

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}
 
std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    return ltrim(rtrim(str, chars), chars);
}

// void trim(string& ref)
// {
// 	int end;
// 	bool flag = true;
// 	while (flag&&ref.length()>0)
// 	{
// 		end = ref.back();
// 		if (end != ' ')
// 			flag = false;
// 		else
// 			ref.pop_back();
// 		end--;
// 	}
// 	flag = true;
// 	int i = 0;
// 	while (flag)
// 		if (ref[i] == ' ')
// 			ref.erase(0, 1);
// 		else
// 			flag = false;
// }