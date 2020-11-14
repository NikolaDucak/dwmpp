FREETYPELIBS = -lfontconfig -lXft
FREETYPEINC = /usr/include/freetype2
INC_DIR = ./inc/

all:
	g++  -std=c++17 ./src/*.cpp  ./src/*/*.cpp -lX11 -lXinerama -I$(INC_DIR) -I$(FREETYPEINC) $(FREETYPELIBS) -o dwmpp -O2

debug:
	g++  -std=c++17 ./src/*.cpp ./src/*/*.cpp -DDEBUG_LOG -lX11 -lXinerama -I$(INC_DIR) -I$(FREETYPEINC) $(FREETYPELIBS) -g -o dwmpp


