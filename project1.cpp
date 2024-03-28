#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <GL/glut.h>

uint8_t n = 6;     /* number of vertices */
float r = 1 / 3.0; /* ratio of distance from point to vertex */
size_t num_iter = 12000; /* number of points */

void myinit()
{

    /* attributes */
    glEnable(GL_BLEND);
    glClearColor(1.0, 1.0, 1.0, 0.0); /* white background */
    glColor3f(1.0, 0.0, 0.0);         /* draw in red */

    /* set up viewing */
    /* 500 x 500 window with origin lower left */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // reset the matrix
    gluOrtho2D(0.0, 500.0, 0.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    /* define a point data type */

    typedef GLfloat point2D[2];

    point2D pol_vertices[n];

    // if n is odd, max vertical distance of first point to the bottom of the polygon
    // is its vertical distance to the bottom line of the polygon.
    // Else, max vertical distnace is 2*radius_y(= 500)
    double radius_y = n % 2 ? 500.0 / (1 + cos(M_PI / n)) : 250.0;
    // First point on the left side of the window must be at distance 500 / 2 from the center
    double radius_x = 250 / abs(sin(2 * M_PI * 1 / n));
    // Vertical distance of center to the bottom line of the polygon
    double offset_y = 500 - radius_y;
    // Horizontal distance of center to the left line of the polygon
    double offset_x = 250.0;

    for (size_t i = 0; i < n; i++)
    {
        pol_vertices[i][1] = offset_y + radius_y * cos(2 * M_PI * i / n);
        pol_vertices[i][0] = offset_x + radius_x * sin(2 * M_PI * i / n);
    }

    point2D p = {250.0, 250.0}; /* Initial point inside the polygon (center of window) */

    glClear(GL_COLOR_BUFFER_BIT); /*clear the window */

    /* computes and plots 5000 new points */

    for (size_t k = 0; k < num_iter; k++)
    {
        uint8_t j = rand() % n; /* pick a vertex at random */

        /* Compute point halfway between vertex and old point */

        p[0] = (r * p[0]) + ((1 - r) * pol_vertices[j][0]);
        p[1] = (r * p[1]) + ((1 - r) * pol_vertices[j][1]);

        /* plot new point */

        glBegin(GL_POINTS);
            glVertex2fv(p);
        glEnd();
    }
    glutSwapBuffers();
    glFlush(); /* clear buffers */
}

enum MENU_TYPE : int
{
    COLOUR = 1,
    HEX,
    PENT,
    EXIT
};

void menu(int choice)
{
    if (choice == COLOUR)
    {
        glColor3f(0.5, 0.5, 0.5);
        glDrawBuffer(GL_BACK);
        display();
    }
    else if (choice == HEX)
    {
        n = 6;
        r = 1 / 3.0;
        num_iter = 12000;
        glDrawBuffer(GL_BACK);
        display();
    }
    else if (choice == PENT)
    {
        n = 5;
        r = 3 / 8.0;
        num_iter = 12000;
        glDrawBuffer(GL_BACK);
        display();
    }
    else if (choice == EXIT)
    {
        exit(0);
    }
};

int main(int argc, char **argv)
{

    /* Standard GLUT initialization */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); /* default, not needed */
    glutInitWindowSize(500, 500);                /* 500 x 500 pixel window */
    glutInitWindowPosition(0, 0);                /* place window top left on display */
    glutCreateWindow("Fractal");                 /* window title */
    glutDisplayFunc(display);                    /* display callback invoked when window opened */

    glutCreateMenu(menu);

    // Add menu items
    glutAddMenuEntry("Colourful", COLOUR);
    glutAddMenuEntry("Hexagon, r=1/3", HEX);
    glutAddMenuEntry("Pentagon, r=3/8", PENT);
    glutAddMenuEntry("Program Terminate", EXIT);

    // Associate a mouse button with menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    myinit(); /* set attributes */

    glutMainLoop(); /* enter event loop */
}
