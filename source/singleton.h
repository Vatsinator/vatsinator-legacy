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

/* Q_ASSERT */
#include <QtGlobal>

template < typename T >
class Singleton {

private:
  static T* __ms_Singleton;

public:
  Singleton() {
    Q_ASSERT(!__ms_Singleton);

    /*
     * This is a little bit complicated, but the best singleton implementation
     * I've ever seen.
     * It counts the relative inherited class's address and stores it in
     * __ms_Singleton. Notice that derived class can inherit from more than one
     * Singleton class, but in that case 'this' of the derived class can differ
     * from 'this' of the Singleton. Solution of this problem is to get
     * non-existent object from address 0x1, casting it to both types and
     * check the difference, which is in fact the distance between
     * Singleton< T > and its derived type T.
     */
    intptr_t offset = (intptr_t)(T*)1 - (intptr_t)(Singleton*)(T*)1;
    __ms_Singleton = (T*)((intptr_t)this + offset);
  }

  ~Singleton() {
    Q_ASSERT(__ms_Singleton);
    __ms_Singleton = 0;
  }

  inline static T &
  getSingleton() {
    Q_ASSERT(__ms_Singleton);
    return *__ms_Singleton;
  }

  inline static T *
  getSingletonPtr() {
    Q_ASSERT(__ms_Singleton);
    return __ms_Singleton;
  }
};

template < typename T >
T* Singleton< T >::__ms_Singleton = 0;

#endif  /* SINGLETON_H */
