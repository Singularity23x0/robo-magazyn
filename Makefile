.TARGET: all

LFLAGS = -Wall -std=c++17
CFLAGS = -c $(LFLAGS)

SRC_PATH = .
BIN_PATH = build
OBJ_PATH = build
BIN_NAME = main

SOURCES = $(wildcard $(SRC_PATH)/*.cpp)
OBJECTS = $(addprefix $(OBJ_PATH)/,$(patsubst %.cpp,%.o,$(notdir $(SOURCES))))

all: $(OBJECTS)
	make prebuild
	g++ $(LFLAGS) $^ -o $(BIN_PATH)/$(BIN_NAME)

prebuild:
	mkdir -p build

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	g++ $(CFLAGS) $^ -o $@

run: all
	$(BIN_PATH)/$(BIN_NAME)

clean:
	rm -frv $(OBJ_PATH)/*.o $(BIN_PATH)/$(BIN_NAME)