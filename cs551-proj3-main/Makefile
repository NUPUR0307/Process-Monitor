TARGET=proc_monitor.a
CC=gcc
CFLAGS=-Wall -g

SOURCES=proc_monitor.c display.c proc_metrics.c proc_data.c 
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

# $(TARGET): $(OBJECTS)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
