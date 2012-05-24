/*
    memorytracker.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "memorytracker.h"

#include <cstring>

#include <iostream>
#include <string>
#include <list>

using std::list;
using std::cout;
using std::endl;

typedef struct {
	myDword	address;
	myDword	size;
	char	file[128];
	myDword	line;
} ALLOC_INFO;

typedef list< ALLOC_INFO * > AllocList;

AllocList* allocList;

void AddTrack(myDword addr, myDword asize, const char* fname, myDword lnum) {
	ALLOC_INFO* info;
	if (!allocList) {
		allocList = new AllocList;
	}
	
	info = new ALLOC_INFO;
	info->address = addr;
	memset(info->file, '\0', 128);
	strncpy(info->file, fname, 127);
	info->line = lnum;
	info->size = asize;
	allocList->insert(allocList->begin(), info);
	
	//cout << "Tracking object created from " << info->file << " at line " << info->line
	//		<< " with address " << info->address << " (" << info->size
	//		<< " B)" << endl;
}

void RemoveTrack(myDword addr) {
	if (!allocList)
		return;
	
	for (AllocList::iterator it = allocList->begin(); it != allocList->end(); ++it) {
		if ((*it)->address == addr) {
			//cout << "Object under address " << (*it)->address << " removed." << endl;
			allocList->remove((*it));
			break;
		}
	}
}

void DumpUnfreed() {
	if (!allocList)
		return;
	
	AllocList::iterator it;
	myDword totalSize = 0;
	
	for (it = allocList->begin(); it != allocList->end(); ++it) {
		cout << (*it)->file << ":\t\tLINE " << (*it)->line << 
			",\t\tADDRESS " << (*it)->address << 
			"\t" << (*it)->size << " unfreed" << endl;
		totalSize += (*it)->size;
	}
	
	cout << "Total unfreed: " << totalSize << endl;
}
