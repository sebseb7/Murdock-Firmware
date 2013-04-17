#ifndef LOG_H_
#define LOG_H_

#include "main.h"

void log_init(void);
void log_printf(const char* text, ...);
void log_sync(void);


#endif
