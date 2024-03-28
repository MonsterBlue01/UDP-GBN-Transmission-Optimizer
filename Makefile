all: bin/myclient bin/myserver

bin/myclient: src/myclient.cc
	mkdir -p bin
	clang++ -std=c++17 -pthread -o bin/myclient src/myclient.cc

bin/myserver: src/myserver.cc
	mkdir -p bin
	clang++ -std=c++17 -pthread -o bin/myserver src/myserver.cc

clean:
	rm -f bin/myclient bin/myserver
