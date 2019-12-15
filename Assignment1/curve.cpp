#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
using namespace std;

void plot(int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


void myInit (void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-100, 100.0, -100, 100.0);
}


void curveAlgo()
{
	int x = 0, y = 0;
    int p = -5;
    // Plotting with base x for x in [0, 2] and using symmetry [-2, 0]
    while (x <= 2) {
        plot(x, y);
        plot(-1 * x, -1 * y);
        if (p >= 0) { // increment y
            y++;
            p += (3 * x * x + 9 * x - 5);
        }
        else {
            p += (3 * x * x + 9 * x + 7);
        }
        x++;
        
    }
    // Plotting with base y for x in [3, 10] and using symmetry [-10, -3]
    p = -55;
    while (x <= 10) {
        plot(x, y);
        plot(-1 * x, -1 * y);
        if (p >= 0) {
            p -= (24 * x * x + 48 * x - 70);
            x++;
        }
        else {
            p += 96;            
        }
        y++;
    }
    return;
}

void coordinate_points() {
    int i = 0;
    while (i <= 100) {
        plot(i, 0);
        plot(-1 * i, 0);
        plot(0, i);
        plot(0, -1 * i);
        i++;
    }
    return;
}

void myDisplay(void)
{
    glClear (GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2i(-100, 0);
    glVertex2i(100, 0);
    glVertex2i(0, 100);
    glVertex2i(0, -100);
    glEnd();
    coordinate_points();

	glColor3f (0.0, 0.0, 0.0);
	glPointSize(3.0);
    curveAlgo();
	glFlush ();
}

int main(int argc, char** argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (1000, 1000);
	glutInitWindowPosition (500, 500);
	glutCreateWindow ("Plot: y = (x^3) / 12");
	glutDisplayFunc(myDisplay);
	myInit ();
	glutMainLoop();
	return 0;
}