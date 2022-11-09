#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <limits.h>
#include <string>
using namespace std;

int32_t readFileFromPath(const string &path, FILE *&fp, uint32_t &fileLength);
void readData(const string &path, void **buffer, size_t &size);

#endif