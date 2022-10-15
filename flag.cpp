#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>

// void draw_circle(GLfloat r, GLfloat x_offset, GLfloat y_offset)
// {
//     glBegin(GL_POLYGON);
//     for (float d = 0; d < 2 * M_PI; d += 0.001)
//     {
//         glVertex2f(x_offset + r * cos(d), y_offset + r * sin(d));
//     }
//     glEnd();
// }

// void draw_star(GLfloat r, GLfloat x_offset, GLfloat y_offset)
// {
//     GLfloat k = sin(M_PI / 10) / sin(M_PI * 7 / 10);
//     GLfloat c = 1;
//     glBegin(GL_POLYGON);
//     for (int d = 0; d < 10; d++)
//     {
//         if (d % 2)
//             c = 1;
//         else
//             c = k;
//         glVertex2f(x_offset + c * r * cos((d * 2 - 1) * M_PI / 10), y_offset + c * r * sin((d * 2 - 1) * M_PI / 10));
//     }
//     glEnd();
// }
class Sphere {
    public:
    Sphere(float r , int slices, int stacks) : r(r),slices(slices),stacks(stacks) {
        vert = new float[slices * stacks * 6 * 3]();
        glColor3f(238.0/255.0, 37.0/255.0, 54.0/255.0);
        for(int i = 0;i < stacks;i++) {
            for(int j = 0;j < slices;j++) {
                glBegin(GL_LINE_LOOP);
                push_point(i,j);
                push_point(i+1,j+1);
                push_point(i,j+1);
                glEnd();
                glBegin(GL_LINE_LOOP);
                push_point(i,j);
                push_point(i+1,j);
                push_point(i+1,j+1);
                glEnd();
            }
        }
        
    }
    void push_point(int i,int j) {
        glVertex3f(
        //  r * cos(i * M_PI / stacks) * cos(j * M_PI / slices),
        //  r * cos(i * M_PI / stacks) * sin(j * M_PI / slices),
        //  r * sin(i * M_PI / stacks));
        r * sin(i * M_PI / stacks) * cos(2 *j * M_PI / slices),
        r * sin(i * M_PI / stacks) * sin(2 *j * M_PI / slices),
        - r * cos(i * M_PI / stacks));
    }

    float * get_vert() { return vert; }
    int get_size() { return slices * stacks * 6 * 3; }

    private:
    float r;
    int slices;
    int stacks;
    int p = 0;
    float * vert;
};

void display(void)
{
    static float t = 0;
    t+=1;
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(238.0/255.0, 37.0/255.0, 54.0/255.0);
    // glRectf(-27, 0, 27, 18);
    // glColor3f(1.0, 1.0, 1.0);
    // glRectf(-27, -18, 27, 0);
    // draw_circle(6.625, -15.595, 9);
    // glColor3f(238.0/255.0, 37.0/255.0, 54.0/255.0);
    // draw_circle(7.25, -11.97, 9);
    glColor3f(1.0, 1.0, 1.0);
    // for (int i = 0; i < 5; i++)
    // {
    //     draw_star(3.45 / 2, -11.97 + 3.8 * cos((i * 4 + 1) * M_PI / 10), 9 + 3.8 * sin((i * 4 + 1) * M_PI / 10));
    // }
    // glPushMatrix();
    
    glPushMatrix();
    glRotatef(1 + 10 *t,0,0,1);
    auto s = Sphere(10,10,10);
    // glutSolidSphere(4,100,100);
    glPopMatrix();
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(540, 360);
    glutCreateWindow("assignment-2 | czh@3200100515");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-27.0, 27.0, -18.0, 18.0, -10.0, 10.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}