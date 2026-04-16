CC = gcc
TARGET = DFBMS
SRCS = main.c 
all: $(SRCS)
	$(CC) -o $(TARGET) $(SRCS)
clean:
	rm -f $(TARGET)
