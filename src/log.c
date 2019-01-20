#include <log.h>

void log_both(const char* message, ...) {
    
}

void log_verbose(const char* message, ...) {

}

void log_info(const char* message, ...) {

}

void log_debug(const char* message, ...) {

}

void log_warning(const char* message, ...) {

}

void log_error(const char* message, ...) {

}

void log_fatal(const char* message, ...) {

}

int match_format_specifier(const char* message) {
    int rv;
    regex_t exp;
    rv = regcomp(&exp, "(the)", REG_EXTENDED | REG_ICASE);
    if (rv != 0) {
        printf("[ERROR] regcomp failed\n");
        return 0;
    }
    int count = match_count(&exp, message);
    regfree(&exp);
    return count;
}

int match_count(regex_t *pexp, char *sz) {
    // we just need the whole string match in this example    
    regmatch_t whole_match;

    // we store the eflags in a variable, so that we can make
    // ^ match the first time, but not for subsequent regexecs
    int eflags = 0;
    int match = 0;
    int match_count = 0;

    size_t offset = 0;
    size_t length = strlen(sz);

    while (regexec(pexp, sz + offset, 1, &whole_match, eflags) == 0) {
        // do not let ^ match again.
        eflags = REG_NOTBOL;
        match = 1;
        match_count++;
        printf("range %zd - %zd matches\n",
               offset + whole_match.rm_so,
               offset + whole_match.rm_eo);

        // increase the starting offset
        offset += whole_match.rm_eo;

        // a match can be a zero-length match, we must not fail
        // to advance the pointer, or we'd have an infinite loop!
        if (whole_match.rm_so == whole_match.rm_eo) {
            offset += 1;
        }

        // break the loop if we've consumed all characters. Note
        // that we run once for terminating null, to let
        // a zero-length match occur at the end of the string.
        if (offset > length) {
            break;
        }
    }
    if (! match) {
        printf("\"%s\" does not contain a match\n", sz);
    }
    return match_count;
}