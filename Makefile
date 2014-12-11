

CC:=g++
CFLAG:= -ggdb -I/usr/local/include
LIBS:= -lm -ldl -L/usr/local/lib -lopencv_calib3d -lopencv_video -lopencv_ml -lopencv_contrib -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui

CPP:=$(wildcard src/*.cpp)
OBJ:=$(addprefix build/,$(notdir $(CPP:.cpp=.o)))

#all: $(OBJ)
#	$(CC) $(LIBS)  $^ -o bin/bkOpenCV
#	
#build/%.o: src/%.cpp
#	$(CC) $(CFLAG) -c -o $@ $<
	
all:
	g++ $(CPP) `pkg-config --libs --cflags opencv` -ldl -o bin/bkOpenCV
	
zed:
	g++ $(CPP) -DZED `pkg-config --libs --cflags opencv` -ldl -o bin/ballPlay
clean:
	rm build/* bin/*
	
#`pkg-config --libs --cflags opencv`
