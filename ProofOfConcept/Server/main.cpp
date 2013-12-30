#include "ServerApp.h"

#undef main
int main(int argc, char **argv)
{
    ServerApp app;
    return app.run();
}