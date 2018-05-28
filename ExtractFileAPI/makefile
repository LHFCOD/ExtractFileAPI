# 指令编译器和选项 
CC=g++
CFLAGS=-Wall -std=c++11 -fPIC -shared -L/usr/lib/x86_64-linux-gnu -lQt5Core -lQt5Xml
# 目标文件
TARGET=libtest.so  
SRCS = interface.cpp ComDocIO.cpp  
INC = -I/usr/include/x86_64-linux-gnu/qt5 
  
OBJS = $(SRCS:.cpp=.o)
$(TARGET):$(OBJS)  
	@echo TARGET:$@  
	@echo OBJECTS:$^  
	$(CC) $(CFLAGS) -o $@ $^  
clean:
	rm -rf $(TARGET) $(OBJS)
%.o:%.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<  
