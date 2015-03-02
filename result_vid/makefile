all:main

INCLUDE = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` -ldl

main:movie_maker.cpp
	g++ movie_maker.cpp -o main $(INCLUDE) $(LIBS)

.PHONY:clean cleanall
clean:
	rm main