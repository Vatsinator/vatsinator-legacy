![Vatsinator](http://vatsim.garapich.pl/images/header.jpg)

**Vatsinator** is an open-source Vatsim network monitor. It shows logged-in pilots and controllers, provides weather and airport information. Vatsinator is designed to be quick and handy, in order to be useful on desktops (when not flying) and note/netbooks (during flight).

Vatsinator uses QT libraries for GUI and OpenGL as a map rendering engine. Currently we can run this application on Linux and Windows. We are looking for MacOS package maintainers.

**To avoid confusion** Vatsinator does NOT show real-life traffic. It is only VATSIM (Virtual Air Traffic Simulation) network monitor. To find out what VATSIM is, see [http://www.vatsim.net](http://www.vatsim.net/).

[![Subscribe in Google+](http://www.vatsim.garapich.pl/images/gplus.png)](https://plus.google.com/100938719910263601185/posts?hl=pl)

---

## Installation

Current release: **Vatsinator 0.2**

- Linux
	-  Ubuntu<br>
		To install Vatsinator using apt, just type in the terminal:
		
		```
		$ sudo add-apt-repository ppa:michal-k93/vatsim
		$ sudo apt-get update
		$ sudo apt-get install vatsinator
		```
		Only 12.04 and 12.10 are currently supported.
	
	- Debian<br>
		There are packages for Debian Wheezy. Squeeze is not supported because it is a little bit outdated.<br>
		[vatsinator-0.2~wheezy_i386.deb](http://vatsinator.garapich.pl/downloads/vatsinator-0.2~wheezy_i386.deb) for i386<br>
		[vatsinator-0.2~wheezy_amd64.deb](http://vatsinator.garapich.pl/downloads/vatsinator-0.2~wheezy_amd64.deb) for amd64<br>
	
	- Fedora<br>
		Builds for Fedora 17:<br>
		[vatsinator-0.2-2.1.i686.rpm](http://vatsinator.garapich.pl/downloads/vatsinator-0.2-2.1.i686.rpm) for i686<br>
		[vatsinator-0.2-2.1.x86_64.rpm](http://vatsinator.garapich.pl/downloads/vatsinator-0.2-2.1.x86_64.rpm) for x86_64<br>
		
	- Gentoo<br>
		You can find Vatsinator ebuild in [regen2-overlay](https://github.com/regen2/regen2-overlay/tree/master/net-misc/vatsinator).<br>
	
	- Arch<br>
		Having AUR set up correctly, Vatsinator installing is as simpe as that:
		
		```
		$ yaourt -S vatsinator
		```
		
		All thanks to ~archtux.<br>
		
		
	- Other distributions<br>
		There are no packages for other distributions, I am afraid. However, you can compile the Source Code (see below).

- Windows<br>
	[VatsinatorSetup-0.2.exe](http://vatsinator.garapich.pl/downloads/VatsinatorSetup-0.2.exe)

- Source Code<br>
	[vatsinator-0.2.tar.gz](http://vatsinator.garapich.pl/downloads/vatsinator-0.2.tar.gz)


## Building
See [wiki](https://github.com/Garrappachc/Vatsinator/wiki/Building-Vatsinator).


## Translations
Available languages:
- English
- Polish

If you want to translate Vatsinator into your own language, visit [Trasifex site](https://www.transifex.com/projects/p/vatsinator/).


## Contact
- Michał Garapich michal@garapich.pl
- Jan Macheta janmacheta@gmail.com


## License
Vatsinator is developed under GPLv3 license. See COPYING file for more information.
