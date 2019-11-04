#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include<stdio.h>
#include<bits/stdc++.h>
#define KEY_ESC 27 /* GLUT doesn't supply this */

int fullscreen = 0;
int mouseDown = 0;

float xrot = 100.0f;
float yrot = -100.0f;

float xdiff = 100.0f;
float ydiff = 100.0f;

float tra_x = 0.0f;
float tra_y = 0.0f;
float tra_z = 0.0f;


float grow_shrink = 70.0f;
float resize_f = 1.0f;

void drawBox()
{

    
    glTranslatef(tra_x, tra_y, tra_z);
    // glColor3f(1.0f,0.0,0.0);
    glBegin(GL_LINE_LOOP);

        glVertex3f(0.5,-0.5,0.0);
        glVertex3f(0.5,0.5,0.0);
        glVertex3f(-0.5,0.5,0.0);
        glVertex3f(-0.5,-0.5,0.0);
    glEnd();
    //draw the nose
    glBegin(GL_LINES);
            
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
    glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    return 1;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
    2.0f, 2.0f, 1.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f);

    glRotatef(xrot, 0.0f, 0.0f, 1.0f);

    drawBox();

    glFlush();
    glutSwapBuffers();
}

void resize(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(grow_shrink, resize_f * w / h, resize_f, 100 * resize_f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void idle(void)
{
    if (!mouseDown)
    {
        xrot += 2.0f;
    }

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

    glutCreateWindow("Assignment 4.");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);

    if (!init())
        return 1;

    glutMainLoop();

    return 0;
}