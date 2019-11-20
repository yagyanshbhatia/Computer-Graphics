#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <bits/stdc++.h>

using namespace std;
int WINDOW_ID; 

struct RGBType {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};
RGBType *pixels; // Contains every pixel on the screen.

int pixelDepths[500*500];

void init(void) {
    
    //Set background color
    glClearColor(1.0, 1.0, 1.0, 0.0);
    
    //Initialize camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 500, 500, 0.0, 0.0, 1.0);
    
    //Initialize RGB array
    pixels = new RGBType[500*500];
    
    // Set everything to white initially.
    for (int i = 0; i < 500*500; i++) {
        pixels[i].r = 1;
        pixels[i].g = 1;
        pixels[i].b = 1;
        pixelDepths[i] = INT_MAX;
    }
}

// Esc to exit.
void keyboard(unsigned char key, int xmouse, int ymouse) {
    switch (key) {
        case 27:
            free(pixels);
            glutDestroyWindow(WINDOW_ID);
            exit(0);
            break;
    }
    glutPostRedisplay();
}

struct sphere{
    int radius;
    int centerx;
    int centery;
    int centerz;
    RGBType color;
};

int distance(int i, int j, sphere r){
    return (int) sqrt ((r.centerx-i)*(r.centerx-i) + (r.centery-j)*(r.centery-j));
}

int depth(int i, int j, sphere s){
    if(distance(i,j,s) > s.radius) return INT_MAX;
    return s.centerz - sqrt(s.radius*s.radius - (i-s.centerx)*(i-s.centerx) - (j-s.centery)*(j-s.centery));
}

int lightx = 1000;
int lighty = 1000;
int lightz = 0;

float getIllumination(int i, int j, int k, sphere s){
    int y = (i-lightx)*(s.centerx-i) + (j-lighty)*(s.centery-j) + (k-lightz)*(s.centerz - k);
    float p = sqrt((i-lightx)*(i-lightx) + (j-lighty)*(j-lighty) + (k-lightz)*(k-lightz));
    float q = sqrt((i-s.centerx)*(i-s.centerx) + (j-s.centery)*(j-s.centery) + (k-s.centerz)*(k-s.centerz));
    return y/p/q;
}

void zbuffer(sphere s){
    for (int i = 0; i < 500; ++i)
    {
        for (int j = 0; j < 500; ++j)
        {
            if(distance(i,j,s) < s.radius){
                if(depth(i,j,s) < pixelDepths[i*500+j]){
                    float x = getIllumination(i,j,depth(i,j,s),s);
                    pixels[i*500+j].r = s.color.r * (0.01+x);
                    pixels[i*500+j].g = s.color.g * (0.01+x);
                    pixels[i*500+j].b = s.color.b * (0.01+x);
                    pixelDepths[i*500+j] = depth(i,j,s);
                }
            }
        }
    }
    glutPostRedisplay();
}

vector<sphere> allSpheres;


void drawSpheres(){
    sphere one = {
        100, //radius
        250, //x
        250, //y
        30, //z
        {1,0,0}, //color of the sphere
    };
    sphere two = {
        50,
        350,
        300,
        60,
        {0,1,0},
    };
    allSpheres.push_back(one);
    allSpheres.push_back(two);
    
    for(auto f: allSpheres){
        zbuffer(f);
    }
}

void display(void) {

    //Clear buffer data
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawSpheres();

    //Draw pixels
    glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
    
    glPointSize(5);
    glColor3f(1.0, 0.0, 1.0);
    
    glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
    glEnd();
    
    //Flush data
    glFlush();
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    
    WINDOW_ID = glutCreateWindow("Assignment 5: Z buffer");
    
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    
    return 0;
    
}