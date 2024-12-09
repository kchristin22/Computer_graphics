#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

GLuint listIndexRect;
GLuint listIndexSquare;
GLuint listIndexTriangle;
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
        // square center (0,5,10)
        square_vertices[i][1] = 5 + 5 * sign(cos(M_PI * i / 2 + M_PI / 2));
        square_vertices[i][0] = 5 * sign(sin(M_PI * i / 2 + M_PI / 2));
        square_vertices[i][2] = 10; // z = 10
    }

    // plots square
    glBegin(GL_QUADS);
    glVertex3fv(square_vertices[0]);
    glVertex3fv(square_vertices[1]);
    glVertex3fv(square_vertices[2]);
    glVertex3fv(square_vertices[3]);
    glEnd();
}

void draw_rectangle()
{
    typedef GLfloat point3D[3];

    // right side of the house: x = 10/2 = 5, y = 10 or 0, z = +- 20/2 = +-10
    point3D vertices[4] = {{5, 10, -10}, {5, 10, 10}, {5, 0, 10}, {5, 0, -10}};

    // plot square
    glBegin(GL_QUADS);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glEnd();
}

void draw_triangle()
{
    typedef GLfloat point3D[3];

    // triangle on top and front of the house
    point3D vertices[3] = {
        {5, 10, 10},
        {0, 10 + 5 * sqrt(3), 10}, // top vertice: x = 0, y = 10 + sin60
                                   // * 10 = 10 + 5 * sqrt(3), z = 10
        {-5, 10, 10}};

    // plot triangle
    glBegin(GL_TRIANGLES);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
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
    glOrtho(-50.0, 50.0, -50.0, 50.0, -50.0,
            50.0); // near = 50.0, far = -50.0, no perspective
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 51, // camera pos, no objects are to be behind the camera
              0, 0, 50, // camera target, z insignificant
              0.0, 1.0, 0.0);

    listIndexRect = glGenLists(1);
    if (!listIndexRect)
    {
        std::cerr << "Rect list wasn't created" << std::endl;
        return;
    }

    glNewList(listIndexRect, GL_COMPILE);
    draw_rectangle();
    glEndList();

    listIndexSquare = glGenLists(1);

    if (!listIndexSquare)
    {
        std::cerr << "Square list wasn't created" << std::endl;
        return;
    }

    glNewList(listIndexSquare, GL_COMPILE);
    draw_square();
    glEndList();

    listIndexTriangle = glGenLists(1);

    if (!listIndexSquare)
    {
        std::cerr << "Triangle list wasn't created" << std::endl;
        return;
    }

    glNewList(listIndexTriangle, GL_COMPILE);
    draw_triangle();
    glEndList();
}
void draw_house()
{
    glColor3f(0.698, 0.13, 0.13); // dark shade of red for the roof
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
                100); // rooftop is metallic (reflective)

    // right top
    glPushMatrix();
    glTranslatef(5, 10, 0.0);     // change coo center to right top of the house
    glRotatef(30, 0.0, 0.0, 1.0); // rotate rectangle (90 - 60) = 30 degrees
    glTranslatef(-5, 0, 0);       // change axis of rectangle
    glCallList(listIndexRect);
    glPopMatrix();

    // left top
    glPushMatrix();
    glTranslatef(-5, 10, 0.0);     // change coo center to left top of the house
    glRotatef(-30, 0.0, 0.0, 1.0); // rotate rectangle (-90 - 60) = -30 degrees
    glTranslatef(-5, 0, 0);        // change axis of rectangle
    glCallList(listIndexRect);
    glPopMatrix();

    // front triangle
    glPushMatrix();
    glCallList(listIndexTriangle);
    glPopMatrix();

    // back triangle
    glPushMatrix();
    glTranslatef(0, 0, -20);
    glCallList(listIndexTriangle);
    glPopMatrix();

    glColor3f(1.0, 0.9725, 0.8627); // light shade of brown for the walls
    GLfloat specular[4] = {0, 0, 0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
                 specular); // no specular highlight
    GLfloat diffuse[4] = {1.0, 0.9725, 0.8627, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);

    // back
    glPushMatrix();
    glTranslatef(0.0, 0.0, -20.0); // square is on z = 10
    glCallList(listIndexSquare);
    glPopMatrix();

    // front
    glPushMatrix();
    glCallList(listIndexSquare);
    glPopMatrix();

    // left side (x = -5)
    glPushMatrix();
    glTranslatef(-10.0, 0.0, 0.0);
    glCallList(listIndexRect);
    glPopMatrix();

    // right side (x = 5)
    glPushMatrix();
    glCallList(listIndexRect);
    glPopMatrix();
}

void draw_surface()
{
    glColor3f(0.0, 0.5, 0.0); // green surface

    glBegin(GL_QUADS);
    glVertex3f(-50, 0, -50);
    glVertex3f(-50, 0, 50);
    glVertex3f(50, 0, 50);
    glVertex3f(50, 0, -50);
    glEnd();
}

void draw_surface_tiles()
{
    glColor3f(0.0, 0.5, 0.0); // green surface

    // Divide the surface into 100 tiles: Each row and column of the divided
    // surface should consist of 10 tiles (10 tiles x 10 tiles = 100). The
    // surface is 100 x 100 (y=0). Hence, each tile is 10 x 10.
    for (int i = 0; i < 100; i++)
    {
        glBegin(GL_QUADS);
        GLint bottom_left[3] = {-50 + (i % 10) * 10, 0, -50 + (i / 10) * 10};
        glVertex3iv(bottom_left);
        glVertex3i(bottom_left[0], 0, bottom_left[2] + 10);      // top left
        glVertex3i(bottom_left[0] + 10, 0, bottom_left[2] + 10); // top right
        glVertex3i(bottom_left[0] + 10, 0, bottom_left[2]);      // bottom right
        glEnd();
    }
}

void display()
{
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    glLoadIdentity();
    // glRotatef(90, 1.0, 0.0, 0.0);
    // glScalef(10, 10, 10);

    draw_house();
    draw_surface();

    glutSwapBuffers();
    glFlush(); // clear buffers
}

void display2()
{
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    glLoadIdentity();
    // glRotatef(90, 1.0, 0.0, 0.0);
    // glScalef(10, 10, 10);

    draw_house();
    draw_surface_tiles();

    glutSwapBuffers();
    glFlush(); // clear buffers
}

void rotate()
{
    theta += 1.0;
    if (theta > 360.0)
        theta -= 360.0;
    // susceptible to the clipping effect
    if (((scale_factor * a) >= 45) && direction == EXPAND)
        direction = DIMINISH;
    else if (scale_factor * a <= 1)
        direction = EXPAND;
    scale_factor =
        direction == EXPAND ? scale_factor + 0.05 : scale_factor - 0.05;
    glutPostRedisplay();
}

void menu_grass(int choice)
{
    if (choice == 0)
    {
        glutDisplayFunc(display);
    }
    else if (choice == 1)
    {
        glutDisplayFunc(display2);
    }
}

int main(int argc, char **argv)
{
    // Standard GLUT initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 1000); // 1000 x 1000 pixel window
    glutInitWindowPosition(0, 0);   // place window top left on display
    glutCreateWindow("The house beyond the pines"); // window title
    glutDisplayFunc(display); // display callback invoked when window opened
    glutIdleFunc(rotate);

    glutCreateMenu(menu_grass);
    // Add menu items
    glutAddMenuEntry("Create grass surface as one entire surface", 0);
    glutAddMenuEntry("Create grass surface as many tiles", 1);

    // Associate a mouse button with menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    myinit(); // set attributes

    glutMainLoop(); // enter event loop
}