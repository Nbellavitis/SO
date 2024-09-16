# Variables
CC = gcc
CFLAGS = -Wall -std=c99 -lm -lrt -pthread -g -D_XOPEN_SOURCE=500
EXECUTABLES = slave md5 view
OBJECTS = slave.o md5.o pipe_master.o view.o shared_memory.o

all: $(EXECUTABLES)
	@echo "Build complete. Cleaning up object files..."
	$(MAKE) clean_objects

slave: slave.o pipe_master.o
	$(CC) $(CFLAGS) -o $@ $^

md5: md5.o pipe_master.o shared_memory.o
	$(CC) $(CFLAGS) -o $@ $^

slave.o: slave.c pipe_master.h
	$(CC) $(CFLAGS) -c $< -o $@

md5.o: md5.c pipe_master.h shared_memory.h
	$(CC) $(CFLAGS) -c $< -o $@

view.o: view.c pipe_master.h shared_memory.h
	$(CC) $(CFLAGS) -c $< -o $@

view: view.o pipe_master.o shared_memory.o
	$(CC) $(CFLAGS) -o $@ $^	
pipe_master.o: pipe_master.c pipe_master.h
	$(CC) $(CFLAGS) -c $< -o $@

clean_objects:
	rm -f $(OBJECTS)

clean:
	rm -f $(EXECUTABLES) salida.txt PVS-Studio.log report.tasks strace_out
	rm -rf .config
	$(MAKE) clean_objects

.PHONY: all clean clean_objects
