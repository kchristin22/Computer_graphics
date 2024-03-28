#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <GL/glut.h>

double round_100(double value)
{
    uint16_t round_val = (uint16_t)round(value);

    if (round_val == 250)
        return 250.0;
    else if (round_val % 5 == 0) // perfect multiple of 5
        return (double)round_val;

    uint16_t rounded_100 = ((round_val + 50) / 100) * 100;
    return (double)rounded_100;
}

void myinit(void)
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

void display(void)
{

    uint8_t n = 6;         /* number of vertices */
    float r = 1 / 3.0; /* ratio of distance from point to vertex */

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
        // std::cout << pol_vertices[i][0] << " " << pol_vertices[i][1] << std::endl;
    }

    point2D p = {250.0, 250.0}; /* Initial point inside the polygon (center of window) */

    glClear(GL_COLOR_BUFFER_BIT); /*clear the window */

    /* computes and plots 5000 new points */

    for (size_t k = 0; k < 12000; k++)
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
    glFlush(); /* clear buffers */
}

int main(int argc, char **argv)
{

    /* Standard GLUT initialization */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); /* default, not needed */
    glutInitWindowSize(500, 500);                /* 500 x 500 pixel window */
    glutInitWindowPosition(0, 0);                /* place window top left on display */
    glutCreateWindow("Sierpinski Gasket");       /* window title */
    glutDisplayFunc(display);                    /* display callback invoked when window opened */

    myinit(); /* set attributes */

    glutMainLoop(); /* enter event loop */
}
