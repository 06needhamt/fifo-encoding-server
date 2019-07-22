#define LOG_LEVEL_CRITICAL "[CRITICAL]"
#define LOG_LEVEL_ERROR "[ERROR]";
#define LOG_LEVEL_WARNING "[WARNING]"
#define LOG_LEVEL_DEBUG "[DEBUG]"
#define LOG_LEVEL_INFO "[INFO]"
#define LOG_LEVEL_VERBOSE "[VERBOSE]"

void log(const char* level, const char* fmt ...);
void log_critical(const char* fmt ...);