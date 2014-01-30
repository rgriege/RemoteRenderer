#include "ServerApp.h"

#undef main
int main(int argc, char **argv)
{
    Server app;
    return app.run();
}