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

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;

void _onMessage(connection_hdl hdl, client::message_ptr msg)
{
    const void* data = msg->get_payload().c_str();
    decoder->decode_frame(static_cast<const uint8_t*>(data), msg->get_payload().length, buffer);
    flip_image_vertically(buffer, width, height);
    glutPostRedisplay();
}

int width = 300;
int height = 300;
int buffer_size;
uint8_t* buffer;
Decoder* decoder;

void display()
{
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glutSwapBuffers();
}

#undef main
int main(int argc, char **argv)
{
    client client;
    client.init_asio();
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    client.clear_access_channels(websocketpp::log::alevel::frame_header);
    client.set_message_handler(bind(&_onMessage, &client, _1, _2));
    websocketpp::lib::error_code ec;
    client::connection_ptr con = client.get_connection("ws://localhost:9001", ec);
    client.connect(con);
    std::thread thread(std::bind(&client::run, &client));

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

    for (;;) {
        glutMainLoopEvent();
    }

    printf("Video playback complete\n");

    glutMainLoop();

    delete decoder;
    free(buffer);

    return EXIT_SUCCESS;
}