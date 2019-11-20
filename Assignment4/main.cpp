#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


#include<bits/stdc++.h>
float xrot = 0.0f;

void drawTetrahedron()
{

    
    glTranslatef(0.0f, 0.0f, 0.0f);
    // glColor3f(0.0f,1.0,0.0);
    glBegin(GL_LINE_LOOP);

        glVertex3f(0.5,-0.5,0.0);
        glVertex3f(0.5,0.5,0.0);
        glVertex3f(-0.5,0.5,0.0);
        glVertex3f(-0.5,-0.5,0.0);
    glEnd();
    //draw the nose
    glBegin(GL_LINES);
        glColor3f(0.0,0.0,1.0); 
        glVertex3f(0.5,-0.5,0.0);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(0.0,0.0,1.0);
        
        glColor3f(0.0,0.0,1.0);
        glVertex3f(0.5,0.5,0.0);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(0.0,0.0,1.0);

        glColor3f(0.0,0.0,1.0);
        glVertex3f(-0.5,0.5,0.0);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(0.0,0.0,1.0);

        glColor3f(0.0,0.0,1.0);
        glVertex3f(-0.5,-0.5,0.0);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(0.0,0.0,1.0);

    glEnd();

}

int init(void)
{
    glClearColor(0.93f, 0.93f, 0.7f, 0.0f);
    return 1;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
    2.0f, 2.0f, -1.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f);

    glRotatef(xrot, 0.0f, 0.0f, 1.0f);

    drawTetrahedron();

    glFlush();
    glutSwapBuffers();
}

void resize(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70.0f, 1.0f, 1.0f, 5.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void idle(void)
{
    xrot += 2.0f;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27 : 
            exit(1); 
            break;
    }

    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitWindowPosition(50, 50);
    glutInitWindowSize(500, 500);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("Assignment 4");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);

    if (!init())
        return 1;

    glutMainLoop();

    return 0;
}