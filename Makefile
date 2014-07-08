ifdef CROSS_COMPILE
	SYSROOT=/home/christopher/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/../arm-linux-gnueabihf/libc
	CC=arm-linux-gnueabihf-c++
	CFLAGS=-c -Wall -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/local/include
else
	CC=/usr/bin/c++
	CFLAGS=-c -Wall -I../include
endif

LDFLAGS= -L../lib -lmosquittopp -lPocoUtil -lPocoFoundation -lMQTTClient
OBJ=obj
BIN=bin
SOURCES=MQTTPubApp.cpp tasks.cpp
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:.cpp=.o))

EXECUTABLE=$(BIN)/mqttpub

.PHONY: all
.PHONY: clean
.PHONY: build

all: $(SOURCES) main $(EXECUTABLE)

main: | $(OBJ)
	$(CC) $(CFLAGS) main.cpp -o $(OBJ)/main.o
	
$(EXECUTABLE): $(OBJECTS) $(OBJ)/main.o | $(BIN)
	$(CC) $(OBJECTS) $(OBJ)/main.o -o $@ $(LDFLAGS)

$(OBJECTS): $(OBJ)/%.o: %.cpp %.h | $(OBJ)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(BIN)

$(OBJ):
	-mkdir $(OBJ)

$(BIN):
	-mkdir $(BIN)

install: $(EXECUTABLE)
	cp $(EXECUTABLE) /usr/local/bin/
