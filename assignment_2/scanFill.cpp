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
    GLfloat xVal; // X value at the minimum vertex, this is where we start our scanline on the x-axis.
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
    
    if (min(vertexOne.second, vertexTwo.second) == vertexOne.second) {
        this->xVal = vertexOne.first;
    } else {
        this->xVal = vertexTwo.first;
    }
    this->slope =
    static_cast<GLfloat>(static_cast<GLfloat>(vertexOne.second - vertexTwo.second))
    / static_cast<GLfloat>((vertexOne.first - vertexTwo.first));
}

// Structs.

// Sort Edges by minY values and, if the same, then sort by minX values.
struct less_than_key
{
    inline bool operator() (const Edges& struct1, const Edges& struct2)
    {
        if (struct1.minY != struct2.minY)
            return (struct1.minY < struct2.minY);
        return (struct1.minX < struct2.minX);
    }
};

// Sorts Edges by current x values.
struct by_x_val_key
{
    inline bool operator() (const Edges& struct1, const Edges& struct2)
    {
        return (struct1.xVal < struct2.xVal);
    }
};

// Stores color data for a single pixel.
struct RGBType {
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

struct colorx { 
    int x,y,z;
}; 
  
colorx colorCode[500*500];

// Globals
int X_DOWN;
GLint scanline;
bool TOGGLE_STATE;
bool MOUSE_STATE;
bool DRAWING; // Enables or disables user input.
bool EDGE_PARITY;
vector<pair<GLint, GLint>> points; // Contains all the vertexes.
vector<Edges> allEdges; // Contains every Edge.
vector<Edges> activeEdges; // Contains Edges currently intersecting the scanline.
RGBType *pixels; // Contains every pixel on the screen.
int numPointsDrawn = 0;

// Draws pixels from (x1, scanline) to (x2, scanline).
void drawPixels(GLfloat x1, GLfloat x2) {
    // round points, not floor, as we want the x values to change with the scanline.
    int i1 = roundf(x1);
    int i2 = roundf(x2);
    int offset = 500 * (500 - scanline);
    for (int i = offset + i1; i < offset + i2; i++) {

        if(numPointsDrawn <=3 )colorCode[i].x = 1;
        else if(numPointsDrawn <=6 )colorCode[i].y = 1;
        else if(numPointsDrawn <=9 )colorCode[i].z = 1;
        int a = colorCode[i].x;
        int b = colorCode[i].y;
        int c = colorCode[i].z;
        cout<< numPointsDrawn << " " << a<< b<<c<<endl;
        int p,q,r;
        switch(a*4+b*2+c)
        {
            case 0:
                p=q=r=0;
                break;
            case 1:
                p=0; q=r=1;
                break;
            case 2:
                p=0; q=r=1;
                break;
            case 3:
                p=r=0; q=1;
                break;
            case 4:
                p=0; q=r=1;
                break;
            case 5:
                p=q=1; r=0;
                break;
            case 6:
                p=q=0; r=1;
                break;
            case 7:
                p=1; q=r=0;
                break;
        } 
        pixels[i].r = p;
        pixels[i].g = q;
        pixels[i].b = r;
        
        glutPostRedisplay();
        // printf("change pixel %d, %d\n", (i1 + i-offset), scanline);
    }
    // printf("Pixels drawn.\n");
}

// Removes edges from the Active Edges if the maxY of the edge is the same as the scanline.
void removeActiveEdgesByScanline() {
    for (vector<Edges>::iterator it = activeEdges.begin(); it < activeEdges.end(); ) {
        if (it->maxY == scanline) {
            activeEdges.erase(it);
        } else {
            it++;
        }
    }
}

// Updates the x values of the Active Edges for the next scanline.
void updateXValues() {
    for (vector<Edges>::iterator it = activeEdges.begin(); it < activeEdges.end(); it++) {
        it->xVal += (1/it->slope);
    }
}

// Adds new Active Edges from All Edges if the scanline reaches their minimum Y value.
void updateActiveEdges() {
    for (vector<Edges>::iterator it = allEdges.begin(); it < allEdges.end(); it++) {
        if (it->minY == scanline) {
            activeEdges.push_back(*it);
        }
        if (it->minY > scanline) {
            return;
        }
    }
}

// Set scanline value to the smallest Y value
void initScanline() {
    if (allEdges.size() != 0) {
        scanline = allEdges.at(0).minY;
        glutPostRedisplay();
    }
}

// Sort Edges to know which order the scanline hits them.
void sortAndFilterEdges() {
    sort(allEdges.begin(), allEdges.end(), less_than_key());
    for (vector<Edges>::iterator it = allEdges.begin(); it < allEdges.end(); it++) {
        // Don't need to deal with horizontal lines, as that's the direction we're scanning.
        if (it->slope == 0) {
            allEdges.erase(it);
        }
    }
}

void sortActiveEdgesByXValues() {
    sort(activeEdges.begin(), activeEdges.end(), by_x_val_key());
}

// Actually fills the polygon, finds the intersections of the edges and the scanline, draws, and updates
// the Edges.
void fillPolygon() {
    // printf("Filling...");
    while (activeEdges.size() != 0) {
        for (vector<Edges>::iterator it = activeEdges.begin(); it < activeEdges.end(); it++) {
            // printf("drawing from %f to %f\n", it->xVal, (it+1)->xVal);
            drawPixels(it->xVal, (it+1)->xVal);
            it++;
        }
        scanline++;
        removeActiveEdgesByScanline();
        updateXValues();
        updateActiveEdges();
        sortActiveEdgesByXValues();
        glutPostRedisplay();
    }
}

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
        colorCode[i].x = 0;
        colorCode[i].y = 0;
        colorCode[i].z = 0;
    }
    
    DRAWING = false;
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
void startFilling() {
    // first and last point forming the final edge. 
    Edges newEdge(points.at(0), points.at(points.size()-1)); 
    allEdges.push_back(newEdge);
    sortAndFilterEdges();
    initScanline();
    updateActiveEdges();
    DRAWING = true;
    glutPostRedisplay();
    fillPolygon();
    glutPostRedisplay();
}

// Stores vertexes on click.
void mouse_down(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN && !DRAWING) {
                // printf("ADDING POINT %d and %d", x, 500 - y);
                numPointsDrawn++;
                points.push_back(pair<GLint, GLint>(x, y));
                if (points.size() > 1) {
                    Edges newEdge(points.at(points.size()-2), points.at(points.size()-1));
                    allEdges.push_back(newEdge);
                }
                if(numPointsDrawn%3 == 0){
                    startFilling();
                    points.clear();
                    activeEdges.clear();
                    allEdges.clear();
                    DRAWING = false;
                }
                glutPostRedisplay();
            }
    }
    glutPostRedisplay();
}

void display(void) {

    //Clear buffer data
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Draw pixels
    glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
    
    glPointSize(5);
    glColor3f(1.0, 0.0, 1.0);
    
    // Draws vertices.
    if (!DRAWING) {
        for (int i = 0; i < points.size(); i++) {
            glBegin(GL_POINTS);
            glVertex2f(points.at(i).first, points.at(i).second);
            glEnd();
        }
    }
    
    // Draws the final polygon outline.
    if (DRAWING) {
        glDrawPixels(500, 500, GL_RGB, GL_FLOAT, pixels);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON);
        glColor3f(0.0, 1.0, 0.0);
        for (int i = 0; i < points.size(); i++) {
            glVertex2f(points.at(i).first, points.at(i).second);
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
    
    WINDOW_ID = glutCreateWindow("Assignment 1: Scan-line fill");
    
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse_down);
    
    glutMainLoop();
    
    return 0;
    
}