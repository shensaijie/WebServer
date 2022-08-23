all: http

http: Main.cpp
	g++ -W -Wall -o http Main.cpp -lpthread

clean:
	rm http
