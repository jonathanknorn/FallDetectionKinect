#include "logger.h"

Logger::Logger(const std::string &file_name):out_file(file_name){}

Logger::~Logger(){
    out_file.close();
}

void Logger::write_int(int n){
    out_file << n << " ";
}

void Logger::write_line(int num, ...){
    va_list arguments;
    va_start(arguments,num);
    double d;
    for (int i = 0; i < num; ++i){
        d = va_arg(arguments, double);
        out_file << d << " ";
    }
    va_end(arguments);
    out_file << "\n";
}

void Logger::write_tab(){
    out_file << "\t";
}
