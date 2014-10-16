
CC := gcc

TARGET := sched
OBJECT := sched.o
SOURCE := sched.c

all: $(TARGET)

$(TARGET) : $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) $(OBJECT)
