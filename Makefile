CXXFLAGS = -O2 -Wall -Werror -std=c++17 -Iinclude

EXE = hw-02_huffman

all: $(EXE)

obj/main.o: src/main.cpp
	mkdir -p obj
	g++ $(CXXFLAGS) -c src/main.cpp -o obj/main.o

obj/statistic.o: src/statistic.cpp
	mkdir -p obj
	g++ $(CXXFLAGS) -c src/statistic.cpp -o obj/statistic.o

obj/coding.o: src/coding.cpp
	mkdir -p obj
	g++ $(CXXFLAGS) -c src/coding.cpp -o obj/coding.o

obj/huffman_tree.o: src/huffman_tree.cpp
	mkdir -p obj
	g++ $(CXXFLAGS) -c src/huffman_tree.cpp -o obj/huffman_tree.o

$(EXE): obj/main.o obj/coding.o obj/huffman_tree.o obj/statistic.o
	mkdir -p obj
	g++ $(CXXFLAGS) obj/main.o obj/coding.o obj/huffman_tree.o obj/statistic.o -o $(EXE)

obj/test.o: src/test.cpp
	mkdir -p obj
	g++ $(CXXFLAGS) -c src/test.cpp -o obj/test.o

test: obj/coding.o obj/huffman_tree.o obj/statistic.o obj/test.o
	mkdir -p obj
	g++ $(CXXFLAGS) obj/coding.o obj/huffman_tree.o obj/statistic.o obj/test.o -o test

clean:
	rm -rf obj/ $(EXE) test
	rm -rf .idea/

.PHONY: clean all

