#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <GL/glut.h>

GLuint listIndex;
GLfloat a = 5;
GLfloat b = 70;
static GLfloat theta = 0.0;
static GLfloat scale_factor = 1;

enum ScaleDirection : uint8_t
{
    EXPAND = 0,
    DIMINISH
};

static enum ScaleDirection direction { EXPAND };

int sign(double value)
{
    if (value == 0)
        return value;
    return value > 0 ? 1 : -1;
}

void draw_square()
{
    typedef GLfloat point3D[3];

    point3D square_vertices[4];

    for (size_t i = 0; i < 4; i++)
    {
        // square center (0,0,0)
        square_vertices[i][1] = sign(cos(M_PI * i / 2 + M_PI / 2));
        square_vertices[i][0] = sign(sin(M_PI * i / 2 + M_PI / 2));
        square_vertices[i][2] = 0; // z = 0
    }

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
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 0.0); // white background

    // set up viewing
    // 500 x 500 window with origin lower left

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // reset the matrix
    glOrtho(-250.0, 250.0, -250.0, 250.0, -250.0, 250.0);
    glMatrixMode(GL_MODELVIEW);

    listIndex = glGenLists(1);

    if (!listIndex)
    {
        std::cerr << "List wasn't created" << std::endl;
        return;
    }

    glNewList(listIndex, GL_COMPILE);
    draw_square();
    glEndList();
}
void draw_cube()
{
    // front
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    glColor3f(0.0, 1.0, 1.0);
    glCallList(listIndex);
    glPopMatrix();
    // down
    glPushMatrix();
    glTranslatef(0.0, -1.0, 0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glColor3f(0.871, 0, 0.98);
    glCallList(listIndex);
    glPopMatrix();
    // back
    glPushMatrix();
    glTranslatef(0.0, 0.0, -1.0);
    glColor3f(0.373, 0, 0.98);
    glCallList(listIndex);
    glPopMatrix();
    // up
    glPushMatrix();
    glTranslatef(0.0, 1.0, 0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glColor3f(0, 0.98, 0.478);
    glCallList(listIndex);
    glPopMatrix();
    // left
    glPushMatrix();
    glTranslatef(-1.0, 0.0, 0.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(0.98, 0.486, 0);
    glCallList(listIndex);
    glPopMatrix();
    // right
    glPushMatrix();
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(1, 0.157, 0.157);
    glCallList(listIndex);
    glPopMatrix();
}

void display()
{
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -b);
    glRotatef(theta, 1.0, 2.0, 2.0);
    glScalef(scale_factor * a, scale_factor * a, scale_factor * a);

    draw_cube();

    glutSwapBuffers();
    glFlush(); // clear buffers
}

void display2()
{
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    glLoadIdentity();
    glRotatef(theta, 1.0, 1.0, 2.0); // use translate instead
    glTranslatef(0.0, 80.0, -8 * b / 10);
    glScalef(scale_factor * a, scale_factor * a, scale_factor * a);

    draw_cube();

    glutSwapBuffers();
    glFlush(); // clear buffers
}

void rotate()
{
    theta += 1.0;
    if (theta > 360.0)
        theta -= 360.0;
    // must: -70 -(scale_factor * a * side)/2 >= - 250 (to fit in the ortho field projection) or scale_factor * a * side <= 500
    if (((scale_factor * a) >= 100) && direction == EXPAND)
        direction = DIMINISH;
    else if (scale_factor * a <= 1)
        direction = EXPAND;
    scale_factor = direction == EXPAND ? scale_factor + 0.05 : scale_factor - 0.05;
    glutPostRedisplay();
}

void menu(int choice)
{
    if (choice == 0)
    {
        glutDisplayFunc(display); // display callback invoked when window opened
    }
    else if (choice == 1)
    {
        glutDisplayFunc(display2); // display callback invoked when window opened
    }
}

int main(int argc, char **argv)
{
    // Standard GLUT initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500); // 500 x 500 pixel window
    glutInitWindowPosition(0, 0); // place window top left on display
    glutCreateWindow("Fractal");  // window title
    glutDisplayFunc(display);     // display callback invoked when window opened
    glutIdleFunc(rotate);

    glutCreateMenu(menu);
    // Add menu items
    glutAddMenuEntry("Rotate around cube's center", 0);
    glutAddMenuEntry("Rotate around (0,0,-56)", 1);

    // Associate a mouse button with menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    myinit(); // set attributes

    glutMainLoop(); // enter event loop
}