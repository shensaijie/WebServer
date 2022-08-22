all: myhttp

myhttp: myhttp.cpp
	gcc -W -Wall -o myhttp myhttp.cpp -lpthread

clean:
	rm myhttp
