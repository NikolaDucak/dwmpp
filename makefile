# Flags
TARGET_EXEC = dwmpp
BUILD_DIR = ./build
SRC_DIR = ./src

SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

INC_FLAGS = -I./inc -I/usr/include/freetype2
LIB_FLAGS = -lX11 -lXinerama -lfontconfig -lXft

CPPFLAGS = -std=c++17 -O2 $(INC_FLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIB_FLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS)  -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)

run: $(BIN)
	@./debug_run.sh

.PHONY: run clean
