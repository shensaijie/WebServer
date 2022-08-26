src=$(wildcard ./*.cpp)  
objs=$(patsubst %.cpp, %.o, $(src))
target=http
$(target):$(objs)
	$(CXX) $(objs) -o $(target) -lpthread
	
%.o:%.cpp
	$(CXX) -c $< -o $@
	
.PHONY:clean
clean:
	rm $(objs) -f
