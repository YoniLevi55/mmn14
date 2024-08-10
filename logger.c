#include <stdarg.h>
#include <stdio.h>

#include "logger.h"

enum LOG_LEVEL log_level = INFO;

void set_log_level(enum LOG_LEVEL level) {
    log_level = level;
}

void logger(enum LOG_LEVEL level, char *msg, ...) {
    if (level < log_level) {
        return;
    }
    va_list args;
    const char *prefix;
    const char *color;

    // Set the prefix and color based on the log level
    switch (level) {
        case INFO:
            prefix = "[INFO]";
            color = "\033[0;32m";  // Green
            break;
        case WARNING:
            prefix = "[WARNING]";
            color = "\033[0;33m";  // Yellow
            break;
        case ERROR:
            prefix = "[ERROR]";
            color = "\033[0;31m";  // Red
            break;
        case DEBUG:
            prefix = "[DEBUG]";
            color = "\033[0;36m";  // Cyan
            break;
        default:
            prefix = "[UNKNOWN]";
            color = "\033[0m";     // Reset to default color
            break;
    }

    // Print the prefix and the message with color
    printf("%s%s ", color, prefix);
    
    // Print the formatted message
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    // Reset the color
    printf("\033[0m");
}