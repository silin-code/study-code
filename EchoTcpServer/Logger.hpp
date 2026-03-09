#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#include <iostream>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include "Mutex.hpp"

namespace NS_LOG_MODULE
{
    enum class LogLevel
    {
        INFO,
        WARING,
        ERROR,
        FATAL,
        DEBUG
    };

    std::string LogLevel2Message(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::INFO:
            return "Info";
        case LogLevel::WARING:
            return "Waring";
        case LogLevel::ERROR:
            return "Error";
        case LogLevel::FATAL:
            return "Fatal";
        case LogLevel::DEBUG:
            return "Debug";
        }
        return nullptr;
    }

    std::string GetCurrentTime()
    {
        struct timeval current_time;
        int n = gettimeofday(&current_time, nullptr);
        (void)n;

        struct tm struct_time;
        localtime_r(&(current_time.tv_sec), &struct_time);

        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%d-%d-%d %d:%d:%d.%ld",
                 struct_time.tm_year + 1900,
                 struct_time.tm_mon + 1,
                 struct_time.tm_mday,
                 struct_time.tm_hour,
                 struct_time.tm_min,
                 struct_time.tm_sec,
                 current_time.tv_usec);
        return timestr;
    }

    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;
    };

    class ConsoleStrategy : public LogStrategy
    {
    public:
        void SyncLog(const std::string &message)
        {
            LockGuard lockguard(_mutex);
            std::cerr << message << std::endl;
        }
        ~ConsoleStrategy() {}

    private:
        Mutex _mutex;
    };

    const std::string defaultpath = "./log";
    const std::string defaultfilename = "log.txt";

    // 文件策略
    class FileLogstrategy : public LogStrategy
    {
    public:
        FileLogstrategy(const std::string &logpath = defaultpath, const std::string &logfilename = defaultfilename)
            : _logpath(logpath),
              _logfilename(logfilename)
        {
            LockGuard lockguard(_mutex);
            if (std::filesystem::exists(_logpath))
                return;
            try
            {

                std::filesystem::create_directories(_logpath);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << '\n';
            }
            if (!_logpath.empty() && _logpath.back() != '/')
            {
                _logpath += '/';
            }
        }
        virtual void SyncLog(const std::string &message) override
        {
            std::string targetlog = _logpath + '/' + _logfilename;
            std::ofstream out(targetlog, std::ios::app);
            if (!out.is_open())
            {
                std::cerr << "open file" << targetlog << std::endl;
                return;
            }
            out << message << "\n";
            out.close();
        }

    private:
        std::string _logpath;
        std::string _logfilename;
        Mutex _mutex;
    };

    class Logger
    {
    public:
        Logger()
        {
        }

        void UseConsoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>();
        }

        void UseFileStrategy()
        {
            _strategy = std::make_unique<FileLogstrategy>();
        }

        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string &filename, int line, Logger &logger)
                : _level(level),
                  _current_time(GetCurrentTime()),
                  _pid(getpid()),
                  _filename(filename),
                  _line(line),
                  _logger(logger)
            {
                std::stringstream ss;
                ss << "[" << _current_time << "]"
                   << "[" << LogLevel2Message(_level) << "]"
                   << "[" << _pid << "]"
                   << "[" << _filename << "]"
                   << "[" << _line << "]";
                _loginfo = ss.str();
            }
            template <class T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();

                return *this;
            }

            ~LogMessage()
            {
                if (_logger._strategy)
                {
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

        private:
            LogLevel _level;
            std::string _current_time;
            pid_t _pid;
            std::string _filename;
            int _line;
            std::string _loginfo;

            Logger &_logger;
        };

        LogMessage operator()(LogLevel level, std::string filename, int line)
        {
            return LogMessage(level, filename, line, *this);
        }

        ~Logger()
        {
        }

    private:
        std::unique_ptr<LogStrategy> _strategy;
    };

    Logger logger;
#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy();
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileStrategy();

#define LOG(level) logger(level, __FILE__, __LINE__)
}

#endif