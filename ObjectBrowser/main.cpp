#include "ObjectBrowser.h"

#undef main
int main(int argc, char **argv)
{
    ObjectBrowser* app;
    if (argc >= 4) {
        std::string remoteHost = argv[1];
        uint16_t remotePort = atoi(argv[2]);
        uint16_t localPort = atoi(argv[3]);
        app = new ObjectBrowser(remoteHost, remotePort, localPort);
    } else {
        app = new ObjectBrowser();
    }

    bool result = app->run();

    delete app;
    return result;
}