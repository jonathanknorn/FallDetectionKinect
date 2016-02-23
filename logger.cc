#include "logger.h"

//Init by opening file stream
Logger::Logger(const std::string &file_name):out_file(file_name){}

Logger::~Logger(){
    //close file stream
    out_file.close();
}

//writes an int to the current line
void Logger::write_int(int n){
    out_file << n << "\t";
}

//writes multiple, tab separated, ints to a line
void Logger::write_line(int num, ...){
    va_list arguments;
    va_start(arguments,num);
    double d;
    for (int i = 0; i < num; ++i){
        d = va_arg(arguments, double);
        out_file << d << "\t";
    }
    va_end(arguments);
    out_file << "\n";
}

//writes multiple, tab separated, doubles to a line
void Logger::write_doubles(int num, ...){
    va_list arguments;
    va_start(arguments,num);
    double d;
    for (int i = 0; i < num; ++i){
        d = va_arg(arguments, double);
        out_file << d << "\t";
    }
    va_end(arguments);
}

//writes a tab to the current line
void Logger::write_tab(){
    out_file << "\t";
}
