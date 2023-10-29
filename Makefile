# Flags do compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Bibliotecas do OpenGL
LIBS=-lglfw -lGLEW -lGL

SRC_DIR = src
INC_DIR = include
UTILS_DIR = utils
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

INCLUDES = -I$(INC_DIR) -I$(UTILS_DIR)

TARGET = my_program

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ $(LIBS) 

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
