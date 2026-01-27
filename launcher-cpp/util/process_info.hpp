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

typedef struct process_info_description
{
    std::string cmd;
} ProcessInfoDescription;

class process_info
{
public:
    static std::vector<ProcessInfoDescription> list();
};


#endif //PROCESSINFO_HPP
