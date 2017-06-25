#include <iostream>
#include "unistd.h"
#include <string.h>
#include "watcher.h"

int main(int argc, char * argv[])
{    
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
        daemon(0, 0);
    return watcher();
}
