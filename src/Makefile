CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -fno-exceptions
TARGET = acaderp
SRCS = grp_86_main.cpp grp_86_Memory.cpp grp_86_Parser.cpp grp_86_Academic.cpp grp_86_Audit.cpp grp_86_Bonus.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp grp_86_erp.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)