#include "ObjectBrowser.h"

#undef main
int main(int argc, char **argv)
{
    if (argc < 4)
        return 1;
    ObjectBrowser app(argv[1], argv[2], argv[3]);
    return app.run();
}