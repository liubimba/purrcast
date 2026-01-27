//
// Created by bimba on 11/17/25.
//

#include "process_info.hpp"


std::vector<ProcessInfoDescription> process_info::list()
{
    std::vector<ProcessInfoDescription> processes;
#if defined(__linux__)
    DIR* dir;
    struct dirent* entry;
    dir = opendir("/proc");
    while ((entry = readdir(dir)) != nullptr)
    {
        if (std::all_of(entry->d_name, entry->d_name + strlen(entry->d_name), ::isdigit))
        {
            std::string commPath = "/proc/" + std::string(entry->d_name) + "/comm";
            std::ifstream commFile(commPath);
            if (commFile.is_open())
            {
                std::string processName;
                std::getline(commFile, processName);
                ProcessInfoDescription description;
                description.cmd = processName;
                processes.push_back(description);
                commFile.close();
            }
        }
    }
#endif
    return processes;
}
