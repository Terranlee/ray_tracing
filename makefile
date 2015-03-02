all:main

OBJECT = homework.o equipment.o image.o vec.o world.o 

INCLUDE = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` -ldl

main:$(OBJECT)
	@echo Linking
	g++ $(OBJECT) -o main $(LIBS)

homework.o:homework.cpp world.h color.h
	@echo Compiling homework.cpp
	g++ -c homework.cpp $(INCLUDE)
equipment.o:equipment.cpp equipment.h
	@echo Compiling equipment.cpp
	g++ -c equipment.cpp $(INCLUDE)
image.o:image.cpp image.h
	@echo Compiling image.cpp
	g++ -c image.cpp $(INCLUDE)
vec.o:vec.cpp vec.h
	@echo Compiling vec.cpp
	g++ -c vec.cpp $(INCLUDE)
world.o:world.cpp world.h
	@echo Compiling world.cpp
	g++ -c world.cpp $(INCLUDE)

.PHONY:clean cleanall
clean:
	rm OBJECT
cleanall:
	rm OBJECT main