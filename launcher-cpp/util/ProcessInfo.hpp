//
// Created by bimba on 11/17/25.
//

#ifndef PROCESSINFO_HPP
#define PROCESSINFO_HPP

#include <string>
#include <vector>

#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <fstream>

typedef struct ProcessInfoDescription
{
    std::string cmd;
} ProcessInfoDescription;

class ProcessInfo
{
public:
    static std::vector<ProcessInfoDescription> list();
};


#endif //PROCESSINFO_HPP
