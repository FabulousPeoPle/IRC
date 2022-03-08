
#ifndef SSPLIT_H
#define SSPLIT_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>

int ssplit(std::string str, std::string delim, std::queue<std::string> &msg);
int ssplit(std::string str, std::string delim, std::vector<std::string> &msg);

#endif
