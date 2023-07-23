CC = g++
CFLAGS  = -g -Wall

main: main.o board.o board_generator.o solver.o solution_dictionary.o variable.o
	$(CC) $(CFLAGS) -o main main.o board.o board_generator.o solver.o solution_dictionary.o variable.o
 
main.o: main.cpp board.hpp board_generator.hpp solver.hpp solution_dictionary.hpp variable.hpp
	$(CC) $(CFLAGS) -c main.cpp

board_generator.o: board_generator.hpp
board.o: board.hpp
solver.o: solver.hpp
solution_dictionary.o: solution_dictionary.hpp
variable.o: variable.hpp

clean:
	rm main
	rm *.o