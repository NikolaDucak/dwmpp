# DWMPP

DWMPP is a work in progress window manager whose features 
are inspired by DWM and some of DWM's more popular patches. 

Like DWM, it comes with built-in bar and configuration is done
by editing the source code and recompiling. But unlike DWM it does
not follow the suckless philosophy of not exceeding 2000 SLOC and is 
written in C++.

DWMPP is currently in deep stages of development and does not provide
all the neccesary features for a window manager!

## Building
Dependencies:
- xorg
- make
- g++-8
- libx11-deb
- libxfg-dev
- libxinerma-dev

If you want to use `debug_run.sh` script debuging/testing you need to have:
- xephyr


You can install them with the following command on a Debian based distro:
```bash
sudo apt install xorg make g++-8 libx11-dev libxft-dev libxinerama-dev xorg
```
and for xephyr:
```bash
sudo apt install xserver-xephyr
```

After you have sorted out the dependencies `make` will create a binary.

