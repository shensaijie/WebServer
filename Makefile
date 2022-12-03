SOURCE   := $(wildcard ./*.cpp)
DEPS     := $(wildcard ./*.h)
OBJS     := $(patsubst %.cpp, %.o, $(SOURCE))

TARGET   := http
CXX      := g++
CXXFLAGS := -g -std=c++11 -Wall
LIBS     := -pthread

all: $(TARGET)

$(TARGET): $(OBJS)
	@# 链接
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
	
%.o: %.cpp $(DEPS)
	@# 编译，生成.o文件
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	
.PHONY:clean
clean:
	rm -f $(OBJS) *~ core
