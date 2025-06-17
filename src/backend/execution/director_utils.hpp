#ifndef DIRECTORY_UTILS_HPP
#define DIRECTORY_UTILS_HPP

#include<string>
#include<direct.h>
#include<io.h>
#include<sys/stat.h>
using namespace std;
inline bool createDirectoryIfNotExists(const string &path){
    struct _stat info;
    if(_stat(path.c_str(),&info)!=0){
        return _mkdir(path.c_str())==0;
    }
    else if(info.st_mode & _S_IFDIR){
        return true;
    }
    return false;
}

inline bool directoryExists(const string &path){
    struct _stat info;
    return (_stat(path.c_str(),&info)==0)&&(info.st_mode&_S_IFDIR);
}

#endif