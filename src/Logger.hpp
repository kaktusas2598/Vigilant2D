#pragma once

#include <cstdio>
#include <fstream>
#include <mutex>
#include <string>

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error
};

/// Logging macros to simplify logging in the engine
#define VG_DEBUG(msg) Logger::Instance()->debug(msg)
#define VG_INFO(msg)  Logger::Instance()->info(msg)
#define VG_WARN(msg)  Logger::Instance()->warn(msg)
#define VG_ERROR(msg) Logger::Instance()->error(msg)

class Logger {
    public:
        static Logger* Instance() {
            if(s_pInstance == 0) {
                s_pInstance = new Logger();
                return s_pInstance;
            }
            return s_pInstance;
        }

        void debug(const std::string &message) { log(LogLevel::Debug, message); }
        void info(const std::string &message) { log(LogLevel::Info, message); }
        void warn(const std::string &message) { log(LogLevel::Warn, message); }
        void error(const std::string &message) { log(LogLevel::Error, message); }

    private:
        Logger() : logFile("debug.log", std::ios::out | std::ios::trunc) {}
        ~Logger() {}

        static Logger* s_pInstance;

        void log(LogLevel level, const std::string &message) {
            std::lock_guard<std::mutex> lock(mutex);

            const char *label = levelToString(level);
            std::fprintf(stderr, "[%s] %s\n", label, message.c_str());

            if (logFile.is_open()) {
                logFile << "[" << label << "] " << message << '\n';
                logFile.flush();
            }
        }

        const char *levelToString(LogLevel level) const {
            switch (level) {
            case LogLevel::Debug:
                return "DEBUG";
            case LogLevel::Info:
                return "INFO";
            case LogLevel::Warn:
                return "WARN";
            case LogLevel::Error:
                return "ERROR";
            default:
                return "LOG";
            }
        }

        std::ofstream logFile;
        std::mutex mutex;
};
