#include <string>
#include <iostream>

#include "LibraryApp.h"
#include "GameApp.h"

enum request_type {
    HELP,
    LIST,
    GAME
};

void showHelp()
{
    std::cout << "Usage: ./DesktopClient [COMMAND]" << std::endl
              << "     --help,             display this help" << std::endl
              << "  -l|--list,             list library titles" << std::endl
              << "  -p|--play=<title>,     play a title" << std::endl;
}

bool isLongArg(std::string arg)
{
    return arg[1] == '-';
}

#undef main
int main(int argc, char **argv)
{
    std::string cmd(argc == 2 ? argv[1] : "--help");
    request_type request;
    std::string title;
    if (cmd == "--help") {
        request = HELP;
    } else if (cmd == "-l" || cmd == "--list") {
        request = LIST;
    } else if (cmd.find("p=", 1) != cmd.npos || cmd.find("play=", 2) != cmd.npos) {
        title = cmd.substr(isLongArg(cmd) ? 6 : 3);
        if (title.length() == 0)
            request = HELP;
        else
            request = GAME;
    }

    ClientApp* app;
    switch (request) {
        case LIST:
            app = new LibraryApp();
            break;
        case GAME:
            app = new GameApp(title);
            break;
        case HELP:
            showHelp();
            return EXIT_SUCCESS;
            break;
    }
    bool result = app->run();
    delete app;
    return result;
}
