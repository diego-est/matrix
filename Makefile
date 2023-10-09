.POSIX:

CXX=clang++
CXXFLAGS=--std=c++20 -Weverything -Werror -pedantic -Wshadow -Weffc++ -Wno-c++98-compat -Wno-sign-conversion
SRC=src/*.cpp
BUILD=build

build: src/main.cpp
	$(CXX) $(SRC) -o $(BUILD)/a.out $(CXXFLAGS)

run: build
	./$(BUILD)/a.out

format:
	clang-format -i $(SRC)

.PHONY: build run format
