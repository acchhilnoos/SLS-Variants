CC = g++
CXXFLAGS = -c -g -Og -std=c++14 -Wall -Wextra -pedantic -MP -MMD

main: main.o
	$(CC) $^ -o $@

clean:
	rm -f main main.o main.d