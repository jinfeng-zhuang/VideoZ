#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
};

#define LOG(lvl, fmt, ...) printf("%s %d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

#endif
