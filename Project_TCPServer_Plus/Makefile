# ###########################
# TCP 服务器 Makefile
# 支持 Select + 智能指针 + C++11
# ###########################

# 编译器指定 g++
CC = g++

# 编译参数：启用 C++11 (必须，支持智能指针/override)
CFLAGS = -std=c++11 -Wall -pthread

# 最终生成的可执行文件名称
TARGET = server

# 所有 .cpp 源文件（严格匹配你的项目路径）
SRC_FILES = \
	src/main.cpp \
	src/log/Logger.cpp \
	src/utils/Utils.cpp \
	src/net/TcpSocket.cpp \
	src/connection/Connection.cpp \
	src/reactor/EventLoop.cpp \
	src/reactor/PollLoop.cpp \
	src/thread/Thread.cpp

# 编译命令：生成可执行文件
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_FILES)
	@echo "==================================="
	@echo "  编译成功！运行 ./server 启动服务"
	@echo "==================================="

# 清理命令：删除编译出的可执行文件
clean:
	rm -f $(TARGET)
	@echo "清理完成！"