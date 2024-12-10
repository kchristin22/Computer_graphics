#include <GL/glut.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

GLuint listIndexRect;
GLuint listIndexSquare;
GLuint listIndexTriangle;
GLuint listIndexSphere;
static GLfloat theta = 0.0;
std::chrono::_V2::steady_clock::time_point lastTime;

#define DIV_SURF_ITER 4

int sign(double value)
{
    if (value == 0)
        return value;
    return value > 0 ? 1 : -1;
}

void set_normal_v(GLfloat a[3], GLfloat b[3])
{
    // (a1​,a2​,a3​)×(b1​,b2​,b3​)=(a2​b3​−a3​b2​,a3​b1​−a1​b3​,a1​b2​−a2​b1​)
    GLfloat normalv[3] = {a[1] * b[2] - a[2] * b[0], a[2] * b[0] - a[0] * b[2],
                          a[0] * b[1] - a[1] * b[0]};
    glNormal3fv(normalv);
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

void draw_sphere()
{
    GLfloat v[4][3] = {{0.0, 0.0, 1.0},
                       {0.0, 0.942809, -0.33333},
                       {-0.816497, -0.471405, -0.333333},
                       {0.816497, -0.471405, -0.333333}};
    int step = pow(2, DIV_SURF_ITER); // Number of steps per edge

    // Normalize points
    auto normalize = [](GLfloat p[3])
    {
        GLfloat magnitude = sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
        for (int i = 0; i < 3; i++)
        {
            p[i] /= magnitude;
        }
    };

    // Face 0: 0, 1, 2
    // Face 1: 1, 2, 3
    // Face 2: 2, 3, 0
    // Face 3: 3, 0, 1
    for (int face = 0; face < 4; face++)
    {
        GLint index[3] = {face, (face + 1) % 4, (face + 2) % 4};

        // Repeat 4 times, which means draw 2^4 = 16 lines from each side (16 x
        // 16 blocks)

        GLfloat p0[3] = {0};
        GLfloat p1[3] = {0};
        GLfloat p2[3] = {0};
        GLfloat p3[3] = {0};

        // Since we know the number of tiles from the beginning we can draw them
        // as we move along two sides of a face:
        //              v1
        //              /_\
        //             /   \
        //   left_dir /     \
        //         ^ /\      \
        //        / /\/\      \
        //       / /\ \/\      \
        //      v0 ------------- v2
        //       -----> right_dir
        // By moving along these lines (directions) (p0 and p1 are subsequent on
        // the left-up line and and p2 resides on the right line) we can get
        // each final small triangle as we move from the base towards the top
        // (first right and then left-up).

        GLfloat left_dir[3] = {(v[index[1]][0] - v[index[0]][0]) / step,
                               (v[index[1]][1] - v[index[0]][1]) / step,
                               (v[index[1]][2] - v[index[0]][2]) / step};

        GLfloat right_dir[3] = {(v[index[2]][0] - v[index[0]][0]) / step,
                                (v[index[2]][1] - v[index[0]][1]) / step,
                                (v[index[2]][2] - v[index[0]][2]) / step};

        for (int i = 0; i < step; i++)
        {
            for (int j = 0; j < step - i; j++) // number of tiles to be painted
                                               // in each diagonal
            {
                for (int k = 0; k < 3; k++)
                {
                    p0[k] = v[index[0]][k] + i * left_dir[k] + j * right_dir[k];
                    p1[k] = p0[k] + left_dir[k];
                    p2[k] = p0[k] + right_dir[k];
                    p3[k] = p0[k] + left_dir[k] + right_dir[k];
                }

                normalize(p0);
                normalize(p1);
                normalize(p2);

                glBegin(GL_TRIANGLES);
                glVertex3fv(p0);
                glVertex3fv(p1);
                glVertex3fv(p2);
                glEnd();

                if (j < (step - i) - 1)
                { // Ensure we don't exceed bounds
                    normalize(p3);

                    glBegin(GL_TRIANGLES);
                    glVertex3fv(p3);
                    glVertex3fv(p1);
                    glVertex3fv(p2);
                    glEnd();
                }
            }
        }
    }
}

// 2nd version of draw_sphere() as a recursive function:
// -------------------------------------------------------------------------------
/*
void draw_sphere_recursive(GLfloat v0[3], GLfloat v1[3], GLfloat v2[3], int
depth)
{
    // Normalize midpoints to project onto sphere
    auto normalize = [](GLfloat p[3])
    {
        GLfloat magnitude = sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
        for (int i = 0; i < 3; i++)
        {
            p[i] /= magnitude;
        }
    };

    if (depth == 0)
    {
        normalize(v0);
        normalize(v1);
        normalize(v2);
        glBegin(GL_TRIANGLES);
        glVertex3fv(v0);
        glVertex3fv(v1);
        glVertex3fv(v2);
        glEnd();
        return;
    }

    // Compute midpoints
    GLfloat midAB[3], midBC[3], midCA[3];
    for (int i = 0; i < 3; i++)
    {
        midAB[i] = (v0[i] + v1[i]) / 2.0f;
        midBC[i] = (v1[i] + v2[i]) / 2.0f;
        midCA[i] = (v2[i] + v0[i]) / 2.0f;
    }

    // Recursively subdivide triangles
    draw_sphere_recursive(v0, midAB, midCA, depth - 1);
    draw_sphere_recursive(v1, midBC, midAB, depth - 1);
    draw_sphere_recursive(v2, midCA, midBC, depth - 1);
    draw_sphere_recursive(midAB, midBC, midCA, depth - 1);
}

void draw_sphere()
{
    // Initial tetrahedron vertices
    GLfloat v[4][3] = {{0.0, 0.0, 1.0},
                       {-0.816497, -0.471405, -0.333333},
                       {0.816497, -0.471405, -0.333333},
                       {0.0, 0.942809, -0.33333}};

    int depth = 4; // Number of subdivisions

    // Subdivide each face of the tetrahedron
    for (int face = 0; face < 4; face++)
        draw_sphere_recursive(v[face], v[(face + 1) % 4], v[(face + 2) % 4],
                           depth);
}
*/
// -------------------------------------------------------------------------------

void myinit()
{

    // attributes
    // GLfloat global_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
    // glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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

    listIndexSphere = glGenLists(1);

    if (!listIndexSphere)
    {
        std::cerr << "Sphere list wasn't created" << std::endl;
        return;
    }

    glNewList(listIndexSphere, GL_COMPILE);
    draw_sphere();
    glEndList();
}

void draw_house()
{
    GLfloat zero[4] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero);

    GLfloat red[4] = {0.698, 0.13, 0.13, 1.0}; // dark shade of red for the roof

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, red);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
                100); // rooftop is metallic (reflective)
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);

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

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
                 zero); // no specular highlight
    GLfloat diffuse[4] = {1.0, 0.91, 0.714,
                          1.0}; // light shade of brown for the walls
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
    GLfloat green[4] = {0.0, 0.5, 0.0, 1.0}; // green surface
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);

    glBegin(GL_QUADS);
    glVertex3f(-50, 0, -50);
    glVertex3f(-50, 0, 50);
    glVertex3f(50, 0, 50);
    glVertex3f(50, 0, -50);
    glEnd();
}

void draw_surface_tiles()
{
    GLfloat green[4] = {0.0, 0.5, 0.0, 1.0}; // green surface
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);

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

void draw_sun()
{
    glRotatef(theta, 0.0, 0.0, -1.0); // z positive axis points towards viewer,
                                      // so we rotate around z negative axis
    glTranslatef(-50.0, 0.0, 0.0);

    GLfloat zero[4] = {0.0, 0.0, 0.0, 1.0};
    GLfloat sun_emission[4] = {1.0, 0.996, 0.7, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_emission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, zero);

    glCallList(listIndexSphere);

    GLfloat dir[4] = {1.0, 0.0, 0.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, zero);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glLightfv(GL_LIGHT0, GL_AMBIENT, zero);
}

void display()
{
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    glLoadIdentity();
    glRotatef(90, 1.0, 0.0, 0.0);
    // glScalef(50, 50, 50);

    draw_house();
    draw_surface();
    draw_sun();

    // glEnable(GL_NORMALIZE);

    glutSwapBuffers();
    glFlush(); // clear buffers
}

void display2()
{
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window
    glLoadIdentity();
    glRotatef(90, 1.0, 0.0, 0.0);
    // glScalef(40, 40, 40);

    draw_house();
    draw_surface_tiles();
    draw_sun();

    // glEnable(GL_NORMALIZE);

    glutSwapBuffers();
    glFlush(); // clear buffers
}

void rotate()
{
    auto currentTime = std::chrono::steady_clock::now();

    double deltaTime =
        std::chrono::duration<double>(currentTime - lastTime).count();

    lastTime = currentTime;

    double rotationSpeed = 10.0; // degrees per second
    theta += rotationSpeed * deltaTime;

    GLfloat intensity = 0;
    if (theta <= 90)
        intensity =
            0.4 +
            (1.0 - 0.4) / 90 *
                theta; // linear interpolation between
                       // points (theta, intensity) = (0, 0.4) and (90, 1.0)
    else
        intensity =
            0.4 -
            (1.0 - 0.4) / 90 *
                (theta -
                 180); // linear interpolation between
                       // points (theta, intensity) = (90, 1.0) and (180, 0.4)

    GLfloat intensityv[4] = {intensity, intensity, intensity, 1.0};

    glLightfv(GL_LIGHT0, GL_SPECULAR, intensityv);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, intensityv);

    // wrap the angle to stay within [0, 180]
    if (theta >= 180.0)
        theta -= 180.0;

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

void menu_shader(int choice)
{
    if (choice == 0)
    {
        glShadeModel(GL_SMOOTH);
    }
    else if (choice == 1)
    {
        glShadeModel(GL_FLAT);
    }
}

int main(int argc, char **argv)
{
    // Standard GLUT initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 1000); // 1000 x 1000 pixel window
    glutInitWindowPosition(0,
                           0); // place window top left on display
    glutCreateWindow("The house beyond the pines"); // window title
    glutDisplayFunc(display); // display callback invoked when window opened

    lastTime = std::chrono::steady_clock::now();

    glutIdleFunc(rotate);

    glutCreateMenu(menu_grass);
    // Add menu items
    glutAddMenuEntry("Create grass surface as one entire surface", 0);
    glutAddMenuEntry("Create grass surface as many tiles", 1);

    // Associate a mouse button with menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutCreateMenu(menu_shader);
    // Add menu items
    glutAddMenuEntry("Smooth shading", 0);
    glutAddMenuEntry("Flat shading", 1);

    // Associate a mouse button with menu
    glutAttachMenu(GLUT_LEFT_BUTTON);

    myinit(); // set attributes

    glutMainLoop(); // enter event loop
}