CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -pthread

Menu: Menu.o DataExtraction.o Calculation.o Gnuplot.o Bootstrap.o
	$(CC) $(CFLAGS) -o Menu Menu.o DataExtraction.o Calculation.o Gnuplot.o Bootstrap.o -lcurl

Menu.o: Menu.cpp DataExtraction.h Calculation.h Group.h Bootstrap.h Gnuplot.h
	$(CC) $(CFLAGS) -c Menu.cpp

Calculation.o: Calculation.h Calculation.cpp
	$(CC) $(CFLAGS) -c Calculation.cpp

DataExtraction.o: DataExtraction.h DataExtraction.cpp Stocks.h Config.h
	$(CC) $(CFLAGS) -c DataExtraction.cpp

Gnuplot.o: Gnuplot.cpp Gnuplot.h Calculation.h
	$(CC) $(CFLAGS) -c Gnuplot.cpp	

Bootstrap.o: Bootstrap.cpp Bootstrap.h Stocks.h
	$(CC) $(CFLAGS) -c Bootstrap.cpp
	
clean:
	rm -rf Menu *.o


