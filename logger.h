enum LOG_LEVEL{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};
void logger(enum LOG_LEVEL level, char *msg, ...);
void set_log_level(enum LOG_LEVEL level);
