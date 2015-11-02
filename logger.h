#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <cstdarg>
#include <string>

class Logger{
public:
    Logger(const std::string &);
    ~Logger();
    void write_int(int n);
    void write_line(int num, ...);
    void write_tab();
private:
    std::ofstream out_file;
};
#endif
