#include "watchdog.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <config-file>" << endl;
        return 1;
    }

    string config_file = argv[1];
    Watchdog watchdog(config_file, 10);

    watchdog.start();

    return 0;
}
