#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <GL/glut.h>

uint8_t n = 6;           /* number of vertices */
float r = 1 / 3.0;       /* ratio of distance from point to vertex */
size_t num_iter = 12000; /* number of points */
bool is_single_colour = true;
bool dynamic = false;
int this_button;         /* the mouse button that was pressed */

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
void delay(int value);

void display_init() {}

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
    glColor3f(1.0, 0.0, 0.0);     /* draw in red */

    glDrawBuffer(GL_BACK);

    /* computes and plots 5000 new points */

    for (size_t k = 0; k < num_iter; k++)
    {
        uint8_t j = rand() % n; /* pick a vertex at random */

        /* Compute point halfway between vertex and old point */

        p[0] = (r * p[0]) + ((1 - r) * pol_vertices[j][0]);
        p[1] = (r * p[1]) + ((1 - r) * pol_vertices[j][1]);

        /* plot new point */

        if (!is_single_colour)
        {
            glColor3f((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
        }

        glBegin(GL_POINTS);
        glVertex2fv(p);
        glEnd();
    }
    glutSwapBuffers();
    glFlush(); /* clear buffers */

    glutDisplayFunc(display_init);       // initiate display callback to do nothing
                                         // (better here than in timer callback to only set it once)
    glutTimerFunc(1000, delay, dynamic); // wait 1 sec to draw next frame if dynamic is true
                                         // else will do nothing
}

void delay(int value)
{
    if (value)
    {
        glutDisplayFunc(display); // change display callback to draw next frame
        glutPostRedisplay();      // request redisplay
    }
};

enum MENU_TYPE : int
{
    COLOUR = 1,
    HEX,
    PENT,
    EXIT
};

void menu(int choice)
{
    glutDisplayFunc(display_init); // when the pop up menu shows up and a redisplay is requested,
                                   // we don't want to call display() again
    if (choice == COLOUR)
    {
        glColor3f(0.5, 0.5, 0.5);
        is_single_colour = false;
        num_iter = 15000;
        dynamic = true;
        display();
    }
    else if (choice == HEX)
    {
        n = 6;
        r = 1 / 3.0;
        num_iter = 12000;
        is_single_colour = true;
        dynamic = false;
        display();
    }
    else if (choice == PENT)
    {
        n = 5;
        r = 3 / 8.0;
        num_iter = 12000;
        is_single_colour = true;
        dynamic = false;
        display();
    }
    else if (choice == EXIT)
    {
        exit(0);
    }
}

void changePos(int x, int y)
{
    if (this_button == GLUT_LEFT_BUTTON) // change view only when left button is pressed
        printf("x: %d, y: %d\n", x, y);
}

void mousePressed(int button, int state, int x, int y)
{
    this_button = button;
}

int main(int argc, char **argv)
{

    srand(time(NULL));

    /* Standard GLUT initialization */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); /* default, not needed */
    glutInitWindowSize(500, 500);                /* 500 x 500 pixel window */
    glutInitWindowPosition(0, 0);                /* place window top left on display */
    glutCreateWindow("Fractal");                 /* window title */
    glutDisplayFunc(display_init);               /* display callback invoked when window opened */

    glutCreateMenu(menu);

    // Add menu items
    glutAddMenuEntry("Colourful", COLOUR);
    glutAddMenuEntry("Hexagon, r=1/3", HEX);
    glutAddMenuEntry("Pentagon, r=3/8", PENT);
    glutAddMenuEntry("Program Terminate", EXIT);

    // Associate a mouse button with menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMotionFunc(changePos);
    glutMouseFunc(mousePressed); // used to detect if the left button is pressed

    myinit(); /* set attributes */

    display();

    glutMainLoop(); /* enter event loop */
}
