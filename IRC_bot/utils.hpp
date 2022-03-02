
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>

int ssplit(std::string str, std::string delim, std::queue<std::string> &msg);
int ssplit(std::string str, std::string delim, std::vector<std::string> &msg);
std::string trim(std::string& s, const char* t);
std::string CurrentTime();
#endif
