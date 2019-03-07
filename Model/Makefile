CFLAGS= -Ofast -Wall
SRC= Metric.cpp LightPath.cpp
HEADER= Metric.h Timer.h LightPath.h Vector.h

all: sample test

sample: $(SRC) $(HEADER) sample.cpp
	g++ sample.cpp $(SRC) -o sample $(CFLAGS)

test: $(SRC) $(HEADER) test.cpp
	g++ test.cpp $(SRC) -o test $(CFLAGS)

clean:
	rm *~ sample test
