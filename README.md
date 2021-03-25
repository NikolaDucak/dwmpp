# DWMPP

DWMPP is a work in progress window manager whose features 
are inspired by DWM and some of DWM's more popular patches. 

Like DWM, it comes with built-in bar and configuration is done
by editing the source code and recompiling. But unlike DWM it does
not follow the suckless philosophy of not exceeding 2000 SLOC and is 
written in C++.

Currently DWMPP is in a usable state, but is rich with bugs.

## Building
Dependencies:
- xorg
- make
- clang++-10
- libx11-dev
- libxfg-dev
- libxinerma-dev

You can install them with the following command on a Debian based distro:
```bash
sudo apt install xorg make g++-8 libx11-dev libxft-dev libxinerama-dev
```
After you have sorted out the dependencies `make` will create a binary.

