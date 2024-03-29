target=redisClient
src=$(wildcard ./src/*.cpp)
obj=$(patsubst %.cpp, %.o, $(src))
cc=g++

lib_path=./lib/
lib_name=redisclient
libSrc_path=./libSrc/

${target} : ${obj}
	cd $(libSrc_path) && make && cd ../
	$(cc) -g -O0 $^ -L $(lib_path) -l$(lib_name) -o $@

.PHONY : clean
clean :
	cd $(libSrc_path) && make clean && cd ../
	rm ${target} ${obj}