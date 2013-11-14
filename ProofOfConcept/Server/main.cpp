#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <GL/freeglut.h>
#include <Encoder.h>
#include <rgbData.h>

#include "Server.h"
#include "Packet.h"
#include "GLWindowStream.h"

#define NUMBEROF(a)   ( ( sizeof( a ) ) / sizeof( a[ 0 ] ) )

/*
 * Some lighting and material values.  Not particularly
 * of interest since this is not a lighting or material
 * demo.  If you must know, though: l0 refers to Light 0;
 * plan refers to the planet,  and pole refers to the flagpole.
 */
GLfloat l0_pos[ ] =  {0, 0, 0, 1};
GLfloat l0_amb[ ]  = {.05f, .05f, .1f, 1};
GLfloat l0_diff[ ] = {.95f, .95f, .8f, 1};

GLfloat plan_diff[ ] = {.15f, .5f, .7f, 1};
GLfloat plan_spec[ ] = {.8f, .9f, 1, 1};

GLfloat pole_diff[ ] = {.8f, .4f, .4f, 1};
GLfloat pole_spec[ ] = {1, 1, 1, 1};

/*
 * A handle on the main window.
 */
int main_window;

int width;
int height;
AVFrame* frame;
Encoder* encoder;

uint8_t* buffer;

GLWindowStream* window_stream;

extern std::atomic<AVPacket> currentFramePacket;
extern std::atomic_bool newFrame;

/*
 * The current font.  Can be any of the builtin fonts.
 */
void *font;

/*
 * Our text message is displayed at the top of a flag.  This
 * symbolic constant is the distance from the core of the planet
 * to the top of the flagpole.
 */
#ifndef FLAG_HEIGHT
#define FLAG_HEIGHT 5
#endif

/*
 * This structure is used to simplify associating a GLUT font
 * with a GLUT menu item.
 */
struct font_map
{
    void *id;
    const char *const name;
};

struct font_map font_map[ ]=
{
    {GLUT_BITMAP_8_BY_13,        "Fixed 8x13"},
    {GLUT_BITMAP_9_BY_15,        "Fixed 9x15"},
    {GLUT_BITMAP_TIMES_ROMAN_10, "Times Roman 10"},
    {GLUT_BITMAP_TIMES_ROMAN_24, "Times Roman 24"},
    {GLUT_BITMAP_HELVETICA_10,   "Helvetica 10"},
    {GLUT_BITMAP_HELVETICA_12,   "Helvetica 12"},
    {GLUT_BITMAP_HELVETICA_18,   "Helvetica 18"},
    {GLUT_STROKE_ROMAN,          "Stroked Roman"},
    {GLUT_STROKE_MONO_ROMAN,     "Stroked Mono Roman"}
};

/*
 * Writes a given string in the current( global ){font}.
 * The string is drawn at the location corresponding to the modelspace
 * origin, providing a somewhat unified interface for the 2 classes
 * of fonts.
 */
void write_string( const char *s )
{
    glDisable( GL_LIGHTING );
    glColor3d( 1, 1, 1 );

    if( ( GLUT_STROKE_ROMAN == font )||( GLUT_STROKE_MONO_ROMAN == font ) )
    {
        /*
         * Stroked fonts are about 100 model units tall.
         * This scales the font down to something usable in the current
         * context.
         */
        static GLfloat f = 1.0/64;
        glScalef( f, f, f );
        glutStrokeString( font, (const unsigned char*)s );
        glScalef( 1/f, 1/f, 1/f );
    }
    else
    {
        glRasterPos3f( 0, 0, 0 );
        glutBitmapString( font, (const unsigned char*)s );
    }
    glEnable( GL_LIGHTING );
}


/*
 * The main window's display callback renders a small bluish world
 * with a flagpole, flying an eerily familiar banner.
 */
void cb_display( void )
{
    int msec = glutGet( GLUT_ELAPSED_TIME );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();
        glRotated( msec / 50.0, 0, 1, 0 );
        glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, plan_diff );
        glMateriali( GL_FRONT, GL_SHININESS, 100 );
        glMaterialfv( GL_FRONT, GL_SPECULAR, plan_spec );
        glutSolidSphere( 3, 20, 20 );
        glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pole_diff );
        glMaterialfv( GL_FRONT, GL_SPECULAR, pole_spec );
        glutSolidCylinder( .1, FLAG_HEIGHT, 6, 3 );
        glTranslated( 0, 0, FLAG_HEIGHT );
        write_string( "Hello\nWorld!" );
    glPopMatrix();

	glPushMatrix();
		std::stringstream ss;
		ss << msec;
		write_string("HEY");
	glPopMatrix();

    glutSwapBuffers( );

	window_stream->read(width*height, buffer);
	//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	flip_image_vertically(buffer, width, height);
	
	AVPacket* pkt = encoder->encodeRgbData(buffer);
	currentFramePacket = *pkt;
	newFrame = true;
    /*fflush(stdout);
	fwrite(pkt->data, 1, pkt->size, ffmpeg);*/
	av_free_packet(pkt);
}

/*
 * All your shape belong to us.
 *
 * Err, sorry.  Nothing especially font-oriented here.
 */
void cb_reshape( int w, int h )
{
    GLdouble r = w * 1.0 / h;
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    if( r < 1 )
        glFrustum( -1, 1, -1/r, 1/r, 3, 100 );
    else
        glFrustum( -r, r, -1, 1, 3, 100 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glLightfv( GL_LIGHT0, GL_POSITION, l0_pos );
    glLightfv( GL_LIGHT0, GL_AMBIENT, l0_amb );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, l0_pos );
    glTranslated( 0, 0, -30 );

    glutPostRedisplay( );
}

/*
 * Idle functions are covered elsewhere.  We just use this one
 * to keep the window updating when nothing else is happening.
 */
void cb_idle( void )
{
    glutSetWindow( main_window );
    glutPostRedisplay( );
}

/*
 * A bit fragile against changes, but maps menu
 * selections to a global {font} setting.
 */
void cb_menu( int item )
{
    if( NUMBEROF( font_map )== item )
        exit( EXIT_SUCCESS );
    else
        font = font_map [ item ].id;
}

#undef main
int main(int argc, char **argv)
{
	newFrame = false;
	Server::getInstance();

    unsigned int i;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DEPTH|GLUT_DOUBLE );
    main_window = glutCreateWindow( "OpenGL Remote Rendering Server" );
    glutDisplayFunc( cb_display );
    glutReshapeFunc( cb_reshape );
    glutIdleFunc( cb_idle );

	window_stream = new GLWindowStream();

    glutCreateMenu( cb_menu );
    for( i = 0; i < NUMBEROF( font_map ); ++i )
        glutAddMenuEntry( font_map [ i ].name, i );
    glutAddMenuEntry( "Quit", i );
    glutAttachMenu( 2 );

	font = GLUT_BITMAP_HELVETICA_12; //GLUT_BITMAP_8_BY_13;

    printf("glut window id: %d\n", glutGetWindow());

	/* ffmpeg init */
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	encoder = new Encoder();
	if (!encoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, width, height, 25))
	//if (!encoder->bootstrap(AV_CODEC_ID_H264, width, height, 25))
		exit(EXIT_FAILURE);
	buffer = new uint8_t[3*width*height];

    for (;;)
    {
        glutPostRedisplay();
        glutMainLoopEvent();
    }

	delete[] buffer;

    return EXIT_SUCCESS;
}