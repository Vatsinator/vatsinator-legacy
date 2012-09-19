![Vatsinator](https://github.com/Garrappachc/Vatsinator/blob/static/images/header.jpg?raw=true)

**Vatsinator** is an open-source Vatsim network monitor. It shows logged-in pilots and controllers, provides weather and airport information. Vatsinator is designed to be quick and handy, in order to be useful on desktops (when not flying) and note/netbooks (during flight).

Vatsinator uses QT libraries for GUI and OpenGL as a map rendering engine. Currently we can run this application on Linux and Windows. We are looking for MacOS package maintainers.

---

## Installation

**Vatsinator 0.1_beta3**

- Linux
	- Ubuntu
		
		To install Vatsinator using apt, just type in the terminal:
		
		```
		$ sudo add-apt-repository ppa:michal-k93/vatsim
		$ sudo apt-get update
		$ sudo apt-get install vatsinator
		```
		
	- Gentoo
		
		You can find Vatsinator ebuild in [regen2-overlay](https://github.com/regen2/regen2-overlay/tree/master/net-misc/vatsinator).
	
	- Arch
		
		Having AUR set up correctly, Vatsinator installing is as simpe as that:
		
		```
		$ yaourt -S vatsinator
		```
		
		All thanks to ~archtux.
		
		
	- Other distributions
		
		There are no packages for other distributions, I am afraid. However, you can compile the Source Code (see below).

- Windows

	[vatsinator-0.1_beta3.exe](https://github.com/downloads/Garrappachc/Vatsinator/vatsinator-0.1_beta3.exe)

- Source Code

	[vatsinator-0.1_beta3.tar.gz](https://github.com/downloads/Garrappachc/Vatsinator/vatsinator-0.1_beta3.tar.gz)


## Compiling
See [wiki](https://github.com/Garrappachc/Vatsinator/wiki/Building-Vatsinator).


## Contact
- Michał Garapich michal@garapich.pl
- Jan Macheta janmacheta@gmail.com

## License
Vatsinator is developed under GPLv3 license. See COPYING file for more information.
