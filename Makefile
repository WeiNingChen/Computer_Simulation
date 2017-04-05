#file name
TARGET=bin/hw1
#compliler
CC=gcc
#library path
LIB_PATH=lib
#include path
INCLUDE_PATH=include
#source path
SOURCE_PATH=src
#library name
ALL_LIB=libsim.a
UTIL_OBJS=simlib.o

#LIBPKS  = protocol entity util


target: main.o $(ALL_LIB)
	@echo ">building hw1..."
	@$(CC) $< $(ALL_LIB) -o $(TARGET) -lm
	@-cp $(ALL_LIB) $(LIB_PATH)
	@-rm -f $(ALL_LIB) $(ENTITY_OBJS) $(UTIL_OBJS) main.o

main.o: $(SOURCE_PATH)/main.c
	@echo ">compiling: main.cpp..."	
	@$(CC) -I$(INCLUDE_PATH) -c -Wall $< 

all:$(ALL_LIB)

libsim.a: $(UTIL_OBJS)
	@echo "Building libentity.a..."	
	@ar rcs $@ $(UTIL_OBJS)

simlib.o: $(SOURCE_PATH)/simlib.c
	@echo ">compiling: simlib.c..."
	@$(CC) -I$(INCLUDE_PATH) -c -Wall -g $< 

install: $(ALL_LIB)
	@-cp $(ALL_LIB) $(LIB_PATH)
	@-rm -f $(ALL_LIB) $(ENTITY_OBJS) $(UTIL_OBJS) main.o

clean:
#	@for lib in $(LIBPKGS); \
	do \
		echo "Cleaning $$lib..."; \
		cd $(SOURCE_PATH)/$$lib; make --no-print-directory PKGNAME=$$lib clean; \
		cd ../..; \
	done
	@echo "Cleaning simlib..."
	@-rm -f src/simlib.o
	@echo "Cleaning main..."
	@-rm -f src/main.o
	@echo "Removing $(LIB_PATH)..."
	@-rm -f lib/libsim.a
	@echo "Removing $(TARGET)..."
	@-rm -f bin/$(TARGET)
	@-rm -f $(TARGET)
