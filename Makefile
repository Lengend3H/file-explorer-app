# Makefile for File Explorer Application
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = file_explorer
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/file_explorer.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

install:
	sudo cp $(TARGET) /usr/local/bin/

.PHONY: clean install