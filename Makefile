INCFLAGS=-I ext/or-tools/include -I ext/simdjson/include
LINKFLAGS=-L ext/or-tools/lib -l ortools -rpath @executable_path/ext/or-tools/lib
CPPFLAGS=--std=c++17 -Os -Wall -Wextra
BIN=SLOP

.PHONY: all
all: ext $(BIN)

ext:
	./install_dependencies.sh

.phony: clean
clean:
	-rm $(BIN)

SLOP: src/main.cpp
	clang++ -o $(BIN) $(INCFLAGS) $(LINKFLAGS) $(CPPFLAGS) src/main.cpp ext/simdjson/src/simdjson.cpp
