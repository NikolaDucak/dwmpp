#include "util/spawn_process.h"

namespace util {

void spawn_process(const std::string& str) {
    if (fork() == 0) {
        static char* args[] { NULL };
        setsid();
        execvp(str.c_str(), args);
        fprintf(stderr, "dwm: execvp %s", str.c_str());
        perror("failed");
        exit(EXIT_SUCCESS);
    }
}

}  // namespace util
