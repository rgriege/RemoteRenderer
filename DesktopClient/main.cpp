#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <GL/freeglut.h>
#include <Decoder.h>
#include <rgbData.h>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>

#include "Config.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;

int width = 300;
int height = 300;
int buffer_size;
uint8_t* buffer;
Decoder* decoder;
std::thread* connectionThread;
enum request_type
{
    LIST,
    PLAY
};
request_type request;
std::string title;

void display()
{
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glutSwapBuffers();
}

void showHelp()
{
    std::cout << "Usage: ./DesktopClient [COMMAND]" << std::endl
              << "     --help,             display this help" << std::endl
              << "  -l|--list,             list library titles" << std::endl
              << "  -p|--play=<title>,     play a title" << std::endl;
}

bool isLongArg(char* arg)
{
    return arg[1] == '-';
}

bool isLongArg(std::string arg)
{
    return arg[1] == '-';
}

void onOpen(client* client, connection_hdl hdl)
{
    client->send(hdl, request == LIST ? "list" : title, websocketpp::frame::opcode::text);
}

void onMessage(connection_hdl hdl, client::message_ptr msg)
{
    if (request == LIST) {
        Config config;
        std::stringstream stream(msg->get_raw_payload());
        config.parse(stream);
        while (config.hasMoreGames()) {
            const Game& game = config.getNextGame();
            std::cout << " - " << game.name << std::endl;
        }
    } else if (request == PLAY) {

    }
}

void _onMessage(connection_hdl hdl, client::message_ptr msg)
{
    const void* data = msg->get_payload().c_str();
    decoder->decode_frame(static_cast<const uint8_t*>(data), msg->get_payload().length(), buffer);
    flip_image_vertically(buffer, width, height);
    glutPostRedisplay();
}

void initConnection()
{
    client client;
    client.init_asio();
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    client.clear_access_channels(websocketpp::log::alevel::frame_header);
    client.set_open_handler(bind(&onOpen, &client, _1));
    client.set_message_handler(bind(&onMessage, _1, _2));
    websocketpp::lib::error_code ec;
    client::connection_ptr con = client.get_connection("ws://localhost:9001", ec);
    client.connect(con);
    connectionThread = new std::thread(std::bind(&client::run, &client));
}

#undef main
int main(int argc, char **argv)
{
    if (argc != 2) {
        showHelp();
    }
    std::string cmd(argv[1]);
    if (cmd == "--help") {
        showHelp();
        return 0;
    } else if (cmd == "-h" || cmd == "--help") {
        request = LIST;
    } else if (cmd.find("p=", 1) != cmd.npos || cmd.find("play=", 1) != cmd.npos) {
        title = cmd.substr(isLongArg(cmd) ? 6 : 3);
        request = PLAY;
    }

    return EXIT_SUCCESS;
}

void doStuff(int argc, char** argv) {
    decoder = new Decoder();
    decoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, width, height);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("OpenGL Remote Rendering Client");
    glutDisplayFunc(display);

    printf("%d\n", glutGetWindow());

    width = glutGet(GLUT_WINDOW_WIDTH);
    height = glutGet(GLUT_WINDOW_HEIGHT);

    buffer_size = width*height*3;
    buffer = (uint8_t*) malloc(buffer_size);

    for (;;)
        glutMainLoopEvent();

    printf("Video playback complete\n");

    glutMainLoop();

    delete decoder;
    free(buffer);
}
