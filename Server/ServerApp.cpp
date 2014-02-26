#include <thread>
#include <stdlib.h>

#include "ServerApp.h"
#include "Config.h"

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

Server::Server()
    : mNextPort(9002)
{
}

bool Server::run()
{
    _loadConfig();
    _initServer();
    std::thread t1(std::bind(&server::run, &mServer));

    std::cout << "Press ENTER to quit..." << std::endl;
    char buf[2];
    std::cin.getline(buf, 1);

    mServer.stop();
    t1.join();

    return true;
}

void Server::_loadConfig()
{
    try {
        mConfig.load("games.cfg");
    } catch (std::exception& e) {
        std::cout << "Error loading config: " << e.what() << std::endl;
    }
}

void Server::_initServer()
{
    mServer.init_asio();
    mServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
    mServer.clear_access_channels(websocketpp::log::alevel::frame_header);
    mServer.set_open_handler(bind(&Server::_onOpen, this, _1));
    mServer.set_close_handler(bind(&Server::_onClose, this, _1));
    mServer.set_message_handler(bind(&Server::_onMessage, this, _1, _2));
    mServer.listen(9001);
    mServer.start_accept();
}

void Server::_onOpen(connection_hdl hdl)
{
    std::cout << "socket opened!" << std::endl;

    std::stringstream os;
    mConfig.stringify(os);
    const std::string& msg = os.str();
    mServer.send(hdl, msg, websocketpp::frame::opcode::binary);
    mConnections.insert(hdl);
}

void Server::_onMessage(connection_hdl hdl, server::message_ptr msg)
{
    server::connection_ptr con = mServer.get_con_from_hdl(hdl);
    std::string name = msg->get_raw_payload();
    if (mConfig.hasGame(name)) {
        std::cout << "Starting '" << name << "' on ports " << mNextPort << " & " << mNextPort+1 << std::endl;
        std::string path = mConfig.lookupGame(name).path;
        std::string directory = path.substr(0, path.rfind('/')+1);

        char remotePort[6];
        itoa(con->get_port(), remotePort, 10);
        std::string remoteHost = con->get_host();

        char localPort[6];
        itoa(mNextPort, localPort, 10);
        mNextPort += 2;
#ifdef _WIN32
        size_t commandLineSize = path.length() + 1 +
               remoteHost.length() + 1 +
               strlen(remotePort) + 1 +
               strlen(localPort) + 1;
        char* commandLine = new char[commandLineSize];
        _snprintf(commandLine, commandLineSize, "%s %s %s %s", path.c_str(), remoteHost.c_str(), remotePort, localPort);

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(path.c_str(), commandLine, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, directory.c_str(), &si, &pi))
            printf("CreateProcess failed (%d).\n", GetLastError());
        delete[] commandLine;
#else
        pid_t pid = vfork();
        if (pid == 0)
            execlp(path.c_str(), remoteHost.c_str(), remotePort, localPort, (char*) NULL);
#endif
        mServer.send(hdl, localPort, websocketpp::frame::opcode::text);
    }
}

void Server::_onClose(connection_hdl hdl)
{
    std::cout << "socket closed!" << std::endl;
    mConnections.erase(hdl);
}

#undef main
int main(int argc, char **argv)
{
    Server app;
    return app.run();
}
