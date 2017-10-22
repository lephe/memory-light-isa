CFLAGS = -Wall -O2 -std=c++11
LDFLAGS = -lSDL2 -lpthread 
EXEC = simu
SRC = $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)
 
all : $(EXEC)
	mv simu ..
 
$(EXEC) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

main.o: main.cpp  screen.h memory.h processor.h types.h
	$(CXX) $(CFLAGS) -o $@ -c $<
processor.o: processor.cpp processor.h types.h
	$(CXX) $(CFLAGS) -o $@ -c $<
memory.o: memory.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<
screen.o: screen.cpp screen.h memory.h types.h
	$(CXX) $(CFLAGS) -o $@ -c $<

#%.o : %.cpp
#	$(CXX) $(CFLAGS) -o $@ -c $<
clean :
	@rm *.o
cleaner : clean
	@rm $(EXEC)

