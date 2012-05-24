/*
    singleton.h

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


#ifndef SINGLETON_H
#define SINGLETON_H

#include <QtGlobal>

template < typename T >
class Singleton {
	
private:
	static T* __ms_Singleton;

public:
	Singleton() {
		Q_ASSERT(!__ms_Singleton);
		
		intptr_t offset = (intptr_t)(T*)1 - (intptr_t)(Singleton *)(T*)1;
		__ms_Singleton = (T*)((intptr_t)this + offset);
	}

	~Singleton() {
		Q_ASSERT(__ms_Singleton);
		__ms_Singleton = 0;
	}

	inline static T& GetSingleton() {
		Q_ASSERT(__ms_Singleton);
		return *__ms_Singleton;
	}

	inline static T* GetSingletonPtr() {
		return __ms_Singleton;
	}
};

template < typename T > T* Singleton <T>::__ms_Singleton = 0;
#endif  /* SINGLETON_H */
