/*
    memorytracker.h
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


#ifndef MEMORYTRACKER_H
#define MEMORYTRACKER_H

#ifndef NO_DEBUG

#include <cstdlib>

typedef unsigned long myDword;

void AddTrack(myDword, myDword, const char*, myDword);
void RemoveTrack(myDword);
void DumpUnfreed();

inline void* operator new(std::size_t size, const char* file, int line) throw() {
  void* ptr = (void*)malloc(size);
  AddTrack((myDword)ptr, size, file, line);
  return ptr;
}

inline void* operator new[](std::size_t size, const char* file, int line) throw() {
  void* ptr = (void*)malloc(size);
  AddTrack((myDword)ptr, size, file, line);
  return ptr;
}

inline void operator delete(void* p) throw() {
  RemoveTrack((myDword)p);
  free(p);
}

inline void operator delete[](void* p) throw() {
  RemoveTrack((myDword)p);
  free(p);
}


#endif // NO_DEBUG

#endif // MEMORYTRACKER_H
