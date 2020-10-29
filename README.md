# DWMPP
----

DWMPP is a work in progress window manager whose features 
are inspired by DWM and some of DWM's more popular patches. 

Like DWM, it comes with built-in bar and configuration is done
by editing the source code and recompiling. But unlike DWM it does
not follow the suckless philosophy of not exceeding 2000 SOLC and is 
written in multiple C++ files insted of single C file.

DWMPP is currently in deep stages of development and currently does not provide
all the neccesary features for a window manager!


## Building
----
Dependencies:
- xorg
- make
- g++
- libx11-deb
- libxfg-dev
- libxinerma-dev

For creating a X session within a window of existing X session for testing, `debug_run.sh` is used after `make` comand generates a binary.
In order for that script to work he needs: 
- xephyr


You can install them with the following command on a Debian based distro:
```bash
sudo apt install make g++ libx11-dev libxft-dev libxinerama-dev xorg
```
and for xephyr:
```bash
sudo apt install xserver-xephyr
```
After you have sorted out the dependencies `make` will create a binary.
Default make recipe is debug build

