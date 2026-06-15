# Makefile for SDLTest

# Define a variable that will be used to indicate if the bin2c command is available.
BIN2C = ../external/bin2c/bin2c

#TODO add compiling on release functionality stripping debug info

all:
# Create data files
	$(BIN2C) data/Ponomar-Regular.ttf data2CFiles/Ponomar-Regular.c CSFontData

# Compile to static library
	mkdir -p dist
	$(CXX) -c packagedData.cpp -O2 -o dist/packagedData.o




clean:
	rm -rf data2CFiles/*
	rm -rf dist
