CXX	:= g++
CXXFLAGS := -std=c++11 -Wall -Wextra

SRCS := RotatingNumbers.cpp
OBJECTS := $(SRCS:%.cpp=%.o)
DEPS := $(OBJECTS:%.o=%.d)

main.exe: $(OBJECTS)
	$(CXX) -o $@ $^

-include $(DEPS)

%.o: %.cpp
	$(CXX) -c -MMD -MP $(CXXFLAGS) $<

pg: $(OBJECTS)
	make clean
	$(CXX) -pg -o $@ $^

clean:
	rm $(OBJECTS)
	rm $(DEPS)
