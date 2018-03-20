main:
	gcc -pipe -std=c++11 -lstdc++ -pthread -lev -lcrypto ./*.hpp ./*.cpp \
	./connection/*.hpp ./connection/*.cpp \
	./interface/*.hpp ./interface/*.cpp \
	./program/*.hpp ./program/*.cpp \
	./socks5/*.hpp ./socks5/*.cpp \
	./utility/*.hpp ./utility/*.cpp
