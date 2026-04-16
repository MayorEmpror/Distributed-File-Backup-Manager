CXX = g++
TARGET = DFBMS
SRCS = main.cpp

all: $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)