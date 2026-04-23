#pragma once//LogLevel.hpp:日志级别枚举和辅助函数头文件
#include <string>
// 日志级别（普通枚举，你能看懂，C++基础）
// 级别从小到大：调试 < 信息 < 警告 < 错误
enum LogLevel {
    DEBUG = 0,
    INFO  = 1,
    WARN  = 2,
    FATAL = 3
};

// 辅助函数：级别转字符串（STL string，复习：函数返回string自动管理内存）
inline std::string levelToString(LogLevel level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO:  return "INFO";
        case WARN:  return "WARN";
        case FATAL: return "FATAL";
        default:    return "UNKNOWN";
    }
}