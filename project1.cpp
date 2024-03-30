#include <stdio.h>
#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <GL/glut.h>

uint8_t n = 6;                        /* number of vertices */
float r = 1 / 3.0;                    /* ratio of distance from point to vertex */
size_t num_iter = 12000;              /* number of points */
bool is_single_colour = true;         /* only paint in red */
bool dynamic = false;                 /* the drawing changes every 1 sec */
int this_button;                      /* the mouse button that was pressed */
int x_pressed, y_pressed;             /* coordinates of where the left mouse button was pressed */
int height;                           /* height of ortho view */
int width;                            /* width of ortho view */
int x_cur_start = 0, y_cur_start = 0; /* coordinates of where the left mouse button was released */

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
    gluOrtho2D(0.0, 1000.0, 0, 1000.0);
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
    double offset_y = 2 * height - 750 + 500 - radius_y;
    // Horizontal distance of center to the left line of the polygon
    double offset_x = 500.0;

    for (size_t i = 0; i < n; i++)
    {
        pol_vertices[i][1] = offset_y + radius_y * cos(2 * M_PI * i / n);
        pol_vertices[i][0] = offset_x + radius_x * sin(2 * M_PI * i / n);
    }

    point2D p = {500.0, 500.0}; /* Initial point inside the polygon (center of ortho coordinates) */

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
    if (this_button == GLUT_LEFT_BUTTON)
    { // change view only when left button is pressed
        // move the view with regards to the current one
        glViewport(x_cur_start + x - x_pressed, y_cur_start + (height - y) - y_pressed, width, height);
        glMatrixMode(GL_MODELVIEW);
        display();
    }
}

void mousePressed(int button, int state, int x, int y)
{
    this_button = button;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    { // save the coordinates of where the left button was pressed
        x_pressed = x;
        y_pressed = height - y;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    { // store the current starting point of the view
        x_cur_start += x - x_pressed;
        y_cur_start += (height - y) - y_pressed;
    }
}

void resize(int w, int h)
{
    height = h;
    width = w;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // reset the matrix
    gluOrtho2D(0, 2 * w, 0, 2 * h);
    glMatrixMode(GL_MODELVIEW);
    display();
}

int main(int argc, char **argv)
{

    if (argc > 1)
    {
        if (argc != 4)
        {
            std::cerr << "Usage: " << argv[0] << " ${n} ${r} ${num_iter}" << std::endl;
            exit(1);
        }
        n = atoi(argv[1]);
        r = std::atof(argv[2]);
        num_iter = atoi(argv[3]);
    }

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

    glutReshapeFunc(resize);

    glutIdleFunc(nullptr);

    myinit(); /* set attributes */

    display();

    glutMainLoop(); /* enter event loop */
}
