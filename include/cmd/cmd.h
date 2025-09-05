//
// Created by zhou_zhengming on 2025/8/20.
//

#ifndef CODECRAFTERS_GIT_CMD_H
#define CODECRAFTERS_GIT_CMD_H

#include <string>
#include <vector>
#include <unordered_map>

typedef struct Cmd{
    std::string name;
    std::vector<std::string> args;
} Cmd;

int switchCmd(Cmd&);


#endif //CODECRAFTERS_GIT_CMD_H
