common = common/controls.o common/shader.o common/texture.o
CC = g++
flag = -g -std=c++11
glFlag = -lGL -lGLEW -lglfw 
output = earth

$(output): earth.o $(common)
		$(CC) earth.o $(common) -o $(output) $(glFlag) $(flag)

$(output).o: main.cpp mesh.h
	$(CC) -c main.cpp -o $(output).o $(flag)


$(common): %.o: %.cpp
	$(CC) -c $< -o $@ $(glFlag) $(flag)

clean:
	rm *.o common/*.o


