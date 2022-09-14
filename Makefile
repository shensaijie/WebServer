src=$(wildcard ./*.cpp)  
objs=$(patsubst %.cpp, %.o, $(src))
target=http
$(target):$(objs)
	$(CXX) -std=c++11 -pthread $(objs) -o $(target)
	
%.o:%.cpp
	$(CXX) -std=c++11 -pthread -c $< -o $@
	
.PHONY:clean
clean:
	rm $(objs) -f
