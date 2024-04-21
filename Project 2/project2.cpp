#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include <time.h>
#include <iostream>
#include <windows.h>
#include <GL/glut.h>

GLuint listIndex;

void draw_square(){
    typedef GLfloat point3D[3];

    point3D square_vertices[4];

    uint8_t a = 5; 

    for (size_t i = 0; i < 4; i++)
    {
        square_vertices[i][1] = 250 + 2 * std::signbit(cos(M_PI * i / 2 + M_PI / 2));
        square_vertices[i][0] = 250 + 2 * std::signbit(sin(M_PI * i / 2 + M_PI / 2));
        square_vertices[i][2] = 1; // z = 1
    }

    glClear(GL_COLOR_BUFFER_BIT); // clear the window
    glColor3f((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);

    // plots square
    glBegin(GL_QUADS);
        glVertex2fv(square_vertices[0]);
        glVertex2fv(square_vertices[1]);
        glVertex2fv(square_vertices[2]);
        glVertex2fv(square_vertices[3]);
    glEnd();
}

void myinit()
{

    // attributes 
    glEnable(GL_BLEND);
    glClearColor(1.0, 1.0, 1.0, 0.0); // white background

    // set up viewing
    // 500 x 500 window with origin lower left

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // reset the matrix
    gluOrtho2D(0.0, 500.0, 0, 500.0);
    glMatrixMode(GL_MODELVIEW);

    listIndex = glGenLists(1);

    if (!listIndex){
        std::cerr << "List wasn't created" << std::endl;
        return; 
    }
    glNewList(listIndex, GL_COMPILE);
        draw_square();
    glEndList();
    
}

void display()
{
    glDrawBuffer(GL_BACK);
    // call the list to draw the initial square at plane z=1
    glCallList(listIndex);

    glutSwapBuffers();
    glFlush(); // clear buffers

}

int main(int argc, char **argv)
{
    // Standard GLUT initialization

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); 
    glutInitWindowSize(500, 500);                // 500 x 500 pixel window
    glutInitWindowPosition(0, 0);                // place window top left on display
    glutCreateWindow("Fractal");                 // window title
    glutDisplayFunc(display);                    // display callback invoked when window opened

    myinit(); // set attributes

    glutMainLoop(); // enter event loop
}