FREETYPELIBS = -lfontconfig -lXft
FREETYPEINC = /usr/include/freetype2
INC_DIR = ./inc/

debug:
	g++  -std=c++17 ./src/*.cpp -lX11 -lXinerama -I$(INC_DIR) -I$(FREETYPEINC) $(FREETYPELIBS) -g -o dwmpp

all:
	g++  -std=c++17 ./src/*.cpp -lX11 -lXinerama -I$(INC_DIR) -I$(FREETYPEINC) $(FREETYPELIBS) -o dwmpp -O3

