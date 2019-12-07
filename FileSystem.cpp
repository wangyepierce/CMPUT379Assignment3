#include "FileSystem.h"
#include<cstring>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
Data d;
char PWD=127;
uint8_t buffer[1024];
ifstream fin;
ofstream fout;
string diskName = "";

string inputfile="in.txt";	//this is the input file in which all the commands are written
ifstream vDisk;	//this is filestream object which actually would be holding the virtual disk file
string fileName; //this is used for fileNames to create,delete, resize etc ..
int fileLength; //that's the length of the file to create *this will behave as block number as well
bool isMounted = false; //true it when you find a consistent file system and mount it on your file system.


void fs_mount(const char * new_disk_name)
{
	if (diskName != "")
	{
		fin.close();
		fout.close();
	}
	fin.open(new_disk_name, ios::binary);
	if (!fin.fail())
	{
		diskName = new_disk_name;
		fin.read((char*)(&d), sizeof(Data));
		d.supBlock.free_block_list[0] |= 1 << 0;
		fout.open(new_disk_name, ios::binary);
		fout.seekp(0, ios::beg);
		fout.write((char*)(&d), sizeof(Data));
		isMounted = true;
	}
	else
		cerr << "Error: Cannot find disk " << new_disk_name<<'\n';
}

void fs_create(const char name[5], int size)
{
	bool isNameDuplicate = false;
	int count = 0;
	for (int i = 0; i < 126; i++)
	{
		if (d.supBlock.inode[i].used_size == 0)
		{
			for (int j = 0; j < 126; j++)
			{
				char temp = d.supBlock.inode[j].dir_parent << 1, pwd = PWD << 1;
				if ((pwd == temp) && (!(strcmp(d.supBlock.inode[j].name, name))))
					isNameDuplicate = true;
			}
			if (!isNameDuplicate)
			{
				int location = 1;
				for (int k = 0; k < 16 && (count < size); k++)
				{
					char temp = d.supBlock.free_block_list[k];
					for (int l = 0; l < 8 && (count < size); l++)
					{
						if ((temp >> l) & 1)
						{
							count = 0;
							location = k * 8 + l + 1;
						}
						else
							count++;
					}
				}
				if (count == size)
				{
					int tempLoc = location % 8, bitCount = 0;
					for (int k = location / 8; k <= (location + size) / 8; k++)
					{
						for (int l = tempLoc; l < 8 && bitCount < size; l++)
						{
							d.supBlock.free_block_list[k] |= 1 << l;
							bitCount++;
						}
						tempLoc = 0;
					}
					strcpy(d.supBlock.inode[i].name, name);
					d.supBlock.inode[i].dir_parent = PWD;
					if (!size)
					{
						d.supBlock.inode[i].dir_parent = d.supBlock.inode[i].dir_parent | 128;
						d.supBlock.inode[i].start_block = 0;
					}
					else
						d.supBlock.inode[i].start_block = location;
					d.supBlock.inode[i].used_size = d.supBlock.inode[i].used_size = size;
					d.supBlock.inode[i].used_size = d.supBlock.inode[i].used_size | 128;
					fout.write((char*)(&d), sizeof(Data));
					fout.seekp(0, ios::beg);
					return;
				}
			}
			else
			{
				cerr << "Error: File or Directory " << name << " already exists\n";
				return;
			}
			if (count < size)
			{
				cerr << "Error: Cannot Allocate " << size << " on " << diskName << '\n';
				return;
			}
		}
	}
	cerr << "Error: Super Block in "<< diskName<<" is full, cannot create " << name<<'\n';
}

void deleteDirectory(char parent)
{
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = parent << 1;
		if (pwd == temp)
		{
			if (!(d.supBlock.inode[i].dir_parent >> 7))
			{
				strcpy(d.supBlock.inode[i].name, "");
				d.supBlock.inode[i].dir_parent = 0;
				char start = d.supBlock.inode[i].start_block, size = d.supBlock.inode[i].used_size << 1;
				size >>= 1;
				d.supBlock.inode[i].start_block = 0;
				d.supBlock.inode[i].used_size = 0;
				for (int j = start; j < start + size; j++)
				{
					for (int k = 0; k < 1024; k++)
					{
						d.data[j][k] = 0;
					}
				}
				int tempLoc = start % 8, bitCount = 0;
				for (int k = start / 8; k <= (start + size) / 8; k++)
				{
					for (int l = tempLoc; l < 8 && bitCount < size; l++)
					{
						d.supBlock.free_block_list[k] ^= 1 << l;
						bitCount++;
					}
					tempLoc = 0;
				}
			}
			else
			{
				deleteDirectory(i);
			}
		}
	}
}

void fs_delete(const char name[5])
{
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = PWD << 1;
		if ((pwd == temp) && (!(strcmp(d.supBlock.inode[i].name, name))))
		{
			if (!(d.supBlock.inode[i].dir_parent >> 7))
			{
				strcpy(d.supBlock.inode[i].name, "");
				d.supBlock.inode[i].dir_parent = 0;
				char start = d.supBlock.inode[i].start_block, size= d.supBlock.inode[i].used_size << 1;
				size >>= 1;
				d.supBlock.inode[i].start_block = 0;
				d.supBlock.inode[i].used_size = 0;
				for (int j = start; j < start + size; j++)
				{
					for (int k = 0; k < 1024; k++)
					{
						d.data[j][k] = 0;
					}
				}
				int tempLoc = start % 8, bitCount = 0;
				for (int k = start / 8; k <= (start + size) / 8; k++)
				{
					for (int l = tempLoc; l < 8 && bitCount < size; l++)
					{
						d.supBlock.free_block_list[k] ^= 1 << l;
						bitCount++;
					}
					tempLoc = 0;
				}
			}
			else
			{
				deleteDirectory(i);
				strcpy(d.supBlock.inode[i].name, "");
				d.supBlock.inode[i].dir_parent = 0;
				d.supBlock.inode[i].used_size = 0;
			}
			fout.write((char*)(&d), sizeof(Data));
			fout.seekp(0, ios::beg);
		}
	}
	cerr << "Error: File or directory " << name << " does not exist\n";
}

void fs_read(const char name[5], int block_num)
{
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = PWD << 1;
		if ((pwd == temp) && (!(strcmp(d.supBlock.inode[i].name, name))))
		{
			if (!(d.supBlock.inode[i].dir_parent >> 7))
			{
				char size = d.supBlock.inode[i].used_size << 1;
				size >>= 1;
				if (block_num < size)
				{
					for (int j = 0; j < 1024; j++)
					{
						buffer[j] = d.data[d.supBlock.inode[i].start_block + block_num][j];
					}
				}
				else
					cerr << "Error: " << name << " does not have block " << block_num<<'\n';
				return;
			}
		}
	}
	cerr << "File " << name << " does not exist\n";
}

void fs_write(const char name[5], int block_num)
{
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = PWD << 1;
		if ((pwd == temp) && (!(strcmp(d.supBlock.inode[i].name, name))))
		{
			if (!(d.supBlock.inode[i].dir_parent >> 7))
			{
				char size = d.supBlock.inode[i].used_size << 1;
				size >>= 1;
				if (block_num < size)
				{
					for (int j = 0; j < 1024; j++)
					{
						d.data[d.supBlock.inode[i].start_block + block_num][j]= buffer[j];
					}
					fout.write((char*)(&d), sizeof(Data));
					fout.seekp(0, ios::beg);
					return;
				}
				else
					cerr << "Error: " << name << " does not have block " << block_num<<'\n';
			}
		}
	}
	cerr << "File " << name << " does not exist\n";
}

void fs_buff(uint8_t buff[1024])
{
	for (int i = 0; i < 1024; i++)
		buffer[i] = 0;
	for (int i = 0; i < 1024; i++)
		buffer[i] = buff[i];
}

void fs_ls(void)
{
	char dot=127, dbDot=127;
	int fileCount = 0;
	if (PWD != 127)
	{
		dot = PWD;
		dbDot = d.supBlock.inode[PWD].dir_parent;
	}
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = dot << 1;
		if (pwd == temp && !(d.supBlock.inode[i].used_size == 0))
			fileCount++;
	}
	cerr << ".    " << fileCount<<'\n';
	fileCount = 0;
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = dbDot << 1;
		if (pwd == temp && !(d.supBlock.inode[i].used_size == 0))
			fileCount++;
	}
	cerr << "..   " << fileCount<<'\n';
	fileCount = 0;
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd= PWD << 1;
		if (pwd == temp && !(d.supBlock.inode[i].used_size == 0))
		{
			if (d.supBlock.inode[i].dir_parent >> 7)
			{
				for (int j = 0; j < 126; j++)
				{
					char temp = d.supBlock.inode[j].dir_parent << 1, pwd = i << 1;
					if (pwd == temp && !(d.supBlock.inode[j].used_size == 0))
						fileCount++;
				}
				cerr << d.supBlock.inode[i].name << "   " << fileCount << '\n';
				fileCount = 0;
			}
			else
			{
				char size= d.supBlock.inode[i].used_size << 1;
				size >>= 1;
				cerr << d.supBlock.inode[i].name << "   " << (int)size << " KB\n";
			}
		}
	}
}

void fs_resize(const char name[5], int new_size)
{
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = PWD << 1;
		if ((pwd == temp) && (!(strcmp(d.supBlock.inode[i].name, name))))
		{
			if (!(d.supBlock.inode[i].dir_parent >> 7))
			{
				char size = d.supBlock.inode[i].used_size << 1;
				size >>= 1;
				if (size >= new_size)
				{
					for (int j = d.supBlock.inode[i].start_block + size - 1; j >= d.supBlock.inode[i].start_block + new_size; j--)
					{
						for (int k = 0; k < 1024; k++)
							d.data[j][k] = 0;
						d.supBlock.free_block_list[j / 8] ^= 1 << (j % 8);
					}
					d.supBlock.inode[i].used_size = new_size;
					d.supBlock.inode[i].used_size |= 128;
					fout.write((char*)(&d), sizeof(Data));
					fout.seekp(0, ios::beg);
					return;
				}
				else if (size < new_size)
				{
					int count = 0;
					for (int j = d.supBlock.inode[i].start_block + size; j < d.supBlock.inode[i].start_block + new_size; j++)
					{
						if (!((d.supBlock.free_block_list[j / 8] >> (j % 8)) & 1))
							count++;
					}
					if (count == (new_size - size))
					{
						for (int j = d.supBlock.inode[i].start_block + size; j < d.supBlock.inode[i].start_block + new_size; j++)
						{
							d.supBlock.free_block_list[j / 8] |= 1 << (j % 8);
						}
						d.supBlock.inode[i].used_size = new_size;
						d.supBlock.inode[i].used_size |= 128;
						fout.write((char*)(&d), sizeof(Data));
						fout.seekp(0, ios::beg);
						return;
					}
					else
					{
						count = 0;
						int location = 0;
						for (int k = 0; k < 16 && (count < new_size); k++)
						{
							char temp = d.supBlock.free_block_list[k];
							for (int l = 0; l < 8 && (count < new_size); l++)
							{
								if ((temp >> l) & 1)
								{
									count = 0;
									location = k * 8 + l + 1;
								}
								else
									count++;
							}
						}
						if (count == new_size)
						{
							int tempLoc = location % 8, bitCount = 0;
							for (int k = location / 8; k <= (location + new_size) / 8; k++)
							{
								for (int l = tempLoc, prevStart = d.supBlock.inode[i].start_block; l < 8 && bitCount < new_size; l++, prevStart++)
								{
									for (int h = 0; h < 1024; h++)
										d.data[k * 8 + l][h] = d.data[prevStart][h];
									d.supBlock.free_block_list[k] |= 1 << l;
									bitCount++;
								}
								tempLoc = 0;
							}

							for (int j = d.supBlock.inode[i].start_block; j < d.supBlock.inode[i].start_block + size; j++)
							{
								for (int k = 0; k < 1024; k++)
									d.data[j][k] = 0;
								d.supBlock.free_block_list[j / 8] ^= 1 << (j % 8);
							}
							d.supBlock.inode[i].start_block = location;
							d.supBlock.inode[i].used_size = new_size;
							d.supBlock.inode[i].used_size |= 128;
							fout.write((char*)(&d), sizeof(Data));
							fout.seekp(0, ios::beg);
							return;
						}
					}
				}
				else
					cerr << "Error: File " << name << " cannot expand to size " << new_size<<'\n';
			}
		}
	}
	cerr << "Error: File " << name << " does not exist\n";
}

void fs_defrag(void)
{
	int order[126];
	for (int i = 0; i < 126; i++)
		order[i] = -1;
	for (int i = 0; i < 126; i++)
	{
		if ((d.supBlock.inode[i].used_size != 0) && (d.supBlock.inode[i].start_block != 0))
			order[d.supBlock.inode[i].start_block] = i;
	}
	int prev = 1, current=-1;
	for (int i = 0; i < 126; i++)
	{
		if (order[i] != -1)
		{
			current = d.supBlock.inode[order[i]].start_block;
			char size = d.supBlock.inode[order[i]].used_size << 1;
			size >>= 1;
			if (prev != current)
			{
				for (int j = prev; j < prev + size; j++)
				{
					int tempLoc = prev % 8, bitCount = 0;
					for (int k = prev / 8; k <= (prev + size) / 8; k++)
					{
						for (int l = tempLoc; l < 8 && bitCount < size; l++, current++)
						{
							for (int h = 0; h < 1024; h++)
								d.data[k * 8 + l][h] = d.data[current][h];
							d.supBlock.free_block_list[k] |= 1 << l;
							bitCount++;
						}
						tempLoc = 0;
					}

					for (int j = d.supBlock.inode[order[i]].start_block; j < d.supBlock.inode[order[i]].start_block + size; j++)
					{
						for (int k = 0; k < 1024; k++)
							d.data[j][k] = 0;
						d.supBlock.free_block_list[j / 8] ^= 1 << (j % 8);
					}
					d.supBlock.inode[i].start_block = prev;
				}
			}
			prev += size;
		}
	}
	fout.write((char*)(&d), sizeof(Data));
	fout.seekp(0, ios::beg);
}

void fs_cd(const char name[5])
{
	if (!(strcmp(".", name)))
		return;
	char parent = d.supBlock.inode[PWD].dir_parent << 1;
	parent >>= 1;
	if (!(strcmp("..", name)))
	{
		PWD = parent;
		return;
	}
	for (int i = 0; i < 126; i++)
	{
		char temp = d.supBlock.inode[i].dir_parent << 1, pwd = PWD << 1;
		if ((pwd == temp) && (!(strcmp(d.supBlock.inode[i].name, name))))
		{
			if (d.supBlock.inode[i].dir_parent >> 7)
				PWD = i;
			return;
		}
	}
	cerr << "Directory " << name << " does not exist\n";
}

void trim(string& ref)
{
	int end;
	bool flag = true;
	while (flag&&ref.length()>0)
	{
		end = ref.back();
		if (end != ' ')
			flag = false;
		else
			ref.pop_back();
		end--;
	}
	flag = true;
	int i = 0;
	while (flag)
		if (ref[i] == ' ')
			ref.erase(0, 1);
		else
			flag = false;
}


void mount(string str, int error_num)
{
	trim(str);
	// cout << "coming to mount" << endl;
	// cout << str << endl;
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
	
	cout << "Enter the input file: "; 
	cin >> inputfile;
	vDisk.open(inputfile);
	string str;
	string command;
	int i = 1;
	while (!vDisk.eof())
	{
		getline(vDisk, str);
		command = str.substr(0, str.find(" "));
		str.erase(0, 2);
		// cout << command << endl;
		// cout << str << endl;
		// if (command.length() > 1)
		// 	cerr << "Error: " << inputfile << ", " << i << endl;
		if (command == "M")
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

