enum LOG_LEVEL{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};
void logger(enum LOG_LEVEL level, char *msg, ...);