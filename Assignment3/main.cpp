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
// Classes.

// Defintion of an edge of our polygon. Contains useful coordinates for the scanline.
class Edges {
public:
    GLint minY;
    GLint maxY;
    GLint minX;
    GLint maxX;
    GLfloat slope;
    // pairs are (x, y).
    Edges(pair<GLint, GLint> vertexOne, pair<GLint, GLint> vertexTwo);
};

// Calculates neccessary values from two vertexes.
Edges::Edges(pair<GLint, GLint> vertexOne, pair<GLint, GLint> vertexTwo) {
    this->minY = min(vertexOne.second, vertexTwo.second);
    this->maxY = max(vertexOne.second, vertexTwo.second);
    this->minX = min(vertexOne.first, vertexTwo.first);
    this->maxX = max(vertexOne.first, vertexTwo.first);

    this->slope =
    static_cast<GLfloat>(static_cast<GLfloat>(vertexOne.second - vertexTwo.second))
    / static_cast<GLfloat>((vertexOne.first - vertexTwo.first));
}

// Stores color data for a single pixel.
struct RGBType {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

bool TOGGLE_STATE;
bool MOUSE_STATE;
bool DRAWING; // Enables or disables user input for POLYGON 1
bool DRAWING2; // Enables or disables user input for POLYGON 1

vector<pair<GLint, GLint>> points; // Contains all the vertexes for POLYGON 1
vector<pair<GLint, GLint>> points2; // Contains all the vertexes for POLYGON 2

vector<Edges> allEdges; // Contains every Edge of POLYGON 1
vector<Edges> allEdges2; // Contains every Edge of POLYGON 2

RGBType *pixels; // Contains every pixel on the screen.

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
    }
    
    DRAWING = false;
    DRAWING2 = false;
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

// Right click to complete polygon and kick off filling process.
void menu(int id) {
    if (id == 1) {
        Edges newEdge(points.at(0), points.at(points.size()-1));
        allEdges.push_back(newEdge);
        DRAWING = true;
        glutPostRedisplay();
    }
    else if (id == 2){
        Edges newEdge(points2.at(0), points2.at(points2.size()-1));
        allEdges2.push_back(newEdge);
        DRAWING2 = true;
        glutPostRedisplay();
    }
    glutPostRedisplay();
}

// Stores vertexes on click.
void mouse_down(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN && !DRAWING) {
                printf("ADDING POINT %d and %d", x, 500 - y);
                points.push_back(pair<GLint, GLint>(x, y));
                if (points.size() > 1) {
                    Edges newEdge(points.at(points.size()-2), points.at(points.size()-1));
                    allEdges.push_back(newEdge);
                }
                glutPostRedisplay();
            }
            if(state == GLUT_DOWN && DRAWING && !DRAWING2) {
                printf("ADDING POINT %d and %d", x, 500 - y);
                points2.push_back(pair<GLint, GLint>(x, y));
                if (points2.size() > 1) {
                    Edges newEdge(points2.at(points2.size()-2), points2.at(points2.size()-1));
                    allEdges2.push_back(newEdge);
                }
                glutPostRedisplay();
            }
    }
    glutPostRedisplay();
}

void display(void) {
    
    //Print OpenGL errors
    GLenum err_code;
    do {
        err_code = glGetError();
        if (err_code != GL_NO_ERROR)
            printf("Error: %s\n", gluErrorString(err_code));
    } while (err_code != GL_NO_ERROR);
    
    //Clear buffer data
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Draw pixels
    glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
    
    glPointSize(5);
    glColor3f(1.0, 0.0, 1.0);
    
    // Draws points.
    if (!DRAWING && !DRAWING2) {
        for (int i = 0; i < points.size(); i++) {
            glBegin(GL_POINTS);
            glVertex2f(points.at(i).first, points.at(i).second);
            glEnd();
        }
    }
    
    // Draws the final polygon outline.
    if (DRAWING && !DRAWING2) {
        glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON);
        for (int i = 0; i < points.size(); i++) {
            glVertex2f(points.at(i).first, points.at(i).second);
        }
        glEnd();
    }

    // Draws points.
    if (DRAWING && !DRAWING2) {
        for (int i = 0; i < points2.size(); i++) {
            glBegin(GL_POINTS);
            glVertex2f(points2.at(i).first, points2.at(i).second);
            glEnd();
        }
    }

    // Draws the final polygon outline.
    if (DRAWING && DRAWING2) {
        glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON);
        for (int i = 0; i < points.size(); i++) {
            glVertex2f(points.at(i).first, points.at(i).second);
        }
        glEnd();
        glBegin(GL_POLYGON);
        for (int i = 0; i < points2.size(); i++) {
            glVertex2f(points2.at(i).first, points2.at(i).second);
        }
        glEnd();
    }
    
    //Flush data
    glFlush();
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    
    WINDOW_ID = glutCreateWindow("Assignment 3: Weiler - Atherton ALgorithm.");
    
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse_down);
    
    glutCreateMenu(menu);
    glutAddMenuEntry("Draw Polygon 1", 1);
    glutAddMenuEntry("Draw Polygon 2 (After 1)", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    
    return 0;
    
}