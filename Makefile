# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g -I./include -I/usr/include/opencv4

# Source files
SRCS = src/core/SimpleISP.cpp test/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = simpleISP

# Main target
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -L/usr/lib/x86_64-linux-gnu \
	-lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui \
	-lopencv_calib3d -lopencv_features2d -lopencv_dnn -lopencv_flann -lopencv_videoio

# Rule for object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)