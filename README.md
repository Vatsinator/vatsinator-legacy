![Vatsinator](https://github.com/Garrappachc/Vatsinator/blob/static/images/header.jpg?raw=true)

**Vatsinator** is an open-source Vatsim network monitor. It shows logged-in pilots and controllers, provides weather and airport information. Vatsinator is designed to be quick and handy, in order to be useful on desktops (when not flying) and note/netbooks (during flight).

Vatsinator uses QT libraries for GUI and OpenGL as a map rendering engine. Currently we can run this application on Linux and Windows. We are looking for MacOS package maintainers.

---

## Downloads

**Vatsinator 0.1_beta2**

- Linux
	- [vatsinator-0.1_beta2-Linux-amd64.deb](https://github.com/downloads/Garrappachc/Vatsinator/vatsinator-0.1_beta2-Linux-x86_64.deb) DEB package (adm64 only)
- Windows
	- [vatsinator-0.1_beta2-win32.zip](https://github.com/downloads/Garrappachc/Vatsinator/vatsinator-0.1_beta2-win32.zip)
- Source Code
	- [vatsinator-0.1_beta2.tar.gz](https://github.com/downloads/Garrappachc/Vatsinator/vatsinator-0.1_beta2.tar.gz)

## Compiling

```
$ git clone git://github.com/Garrappachc/Vatsinator.git
$ cd Vatsinator
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

By default, cmake will install package in /usr/local. To set prefix to /usr, as most distributions prefers, you can append `-DCMAKE_INSTALL_PREFIX=/usr` to cmake.

## Contact
- Michał Garapich michal@garapich.pl
- Jan Macheta jan@macheta.net

## License
Vatsinator is developed under GPL-3 license. See COPYING file for more information.
