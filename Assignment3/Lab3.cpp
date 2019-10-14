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

using namespace std;

struct Point{
    GLint x;
    GLint y;

    bool isIntersection = false;
    bool isInOut = false;

    int distPoly = 0;
    int distClip = 0;
};

vector<Point> polygonPoints;
vector<Point> clipPoints;

vector<Point> polygonIntersections[100];
vector<Point> clipIntersections[100];

vector<Point> polygonCombined;
vector<Point> clipCombined;

vector<vector<Point>> areas;

bool complete = false;

bool drawPoly;
bool drawClip;
Point x1;

bool isEqual(Point A,Point B){
    return A.x==B.x && A.y==B.y;
}

int inArr(vector<Point>v,Point p){
    for(int i=0;i<v.size();i++){
        if(isEqual(p,v[i])){
           return i;
        }
    }
    return -1;
}

bool isPointInsideWindow(Point p) {
	int cnt = 0, size = clipPoints.size();
	for (int i = 0; i < size; i++) {
		Point p1 = clipPoints[i];
		Point p2 = clipPoints[(i + 1) % size];
		if (p1.y == p2.y) continue;
		if (p.y < min(p1.y, p2.y)) continue;
		if (p.y >= max(p1.y, p2.y)) continue;
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		if (x > p.x) cnt++;
	}
	return (cnt % 2 == 1);
}

bool isPointInsidePolygon(Point p){
   	int cnt = 0, size = polygonPoints.size();
	for (int i = 0; i < size; i++) {
		Point p1 = polygonPoints[i];
		Point p2 = polygonPoints[(i + 1) % size];
		if (p1.y == p2.y) continue;
		if (p.y < min(p1.y, p2.y)) continue;
		if (p.y >= max(p1.y, p2.y)) continue;
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		if (x > p.x) cnt++;
	}
	return (cnt % 2 == 1); 
}


int cross(Point& p0, Point& p1, Point& p2) {
	// put p2 in eqn of line passing through p0 and p1,
	// which is (x-p0.x)*(p1.y-p2.y)-(p1.x-p0.x)*(y-p0.y).
	return ((p2.x - p0.x) * (p1.y - p0.y) - (p1.x - p0.x) * (p2.y - p0.y));
}

bool onSegment(Point& p0, Point& p1, Point& p2) {
	int minx = min(p0.x, p1.x), maxx = max(p0.x, p1.x);
	int miny = min(p0.y, p1.y), maxy = max(p0.y, p1.y);
	if (p2.x >= minx && p2.x <= maxx && p2.y >= miny && p2.y <= maxy) return true;
	return false;
}
bool segmentsIntersect(Point& p1, Point& p2, Point& p3, Point& p4) {
	int d1 = cross(p3, p4, p1);
	int d2 = cross(p3, p4, p2);
	int d3 = cross(p1, p2, p3);
	int d4 = cross(p1, p2, p4);
	// If one line clearly seperates other 2 points.
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
			((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
		return true;

	if (d1 == 0 && onSegment(p3, p4, p1)) return true;
	if (d2 == 0 && onSegment(p3, p4, p2)) return true;
	if (d3 == 0 && onSegment(p1, p2, p3)) return true;
	if (d4 == 0 && onSegment(p1, p2, p4)) return true;
	return false;
}

Point getintersectPoint(Point p1, Point p2, Point p3, Point p4){
	Point p;
	int b1 = (p2.y - p1.y) * p1.x + (p1.x - p2.x) * p1.y;
	int b2 = (p4.y - p3.y) * p3.x + (p3.x - p4.x) * p3.y;
	int D = (p2.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p2.y - p1.y);
	int D1 = b2 * (p2.x - p1.x) - b1 * (p4.x - p3.x);
	int D2 = b2 * (p2.y - p1.y) - b1 * (p4.y - p3.y);
	p.x = D1 / D;
	p.y = D2 / D;
	return p;
}

int distance(Point p1,Point p2) {
    return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
}

bool generateIntersectionPoints(){

    bool intersectionPointsPresent = false;

    for(int i=0;i<polygonPoints.size();i++){
        Point p1 = polygonPoints[i];
        Point p2 = polygonPoints[(i+1)%polygonPoints.size()];

        for(int j=0;j<clipPoints.size();j++){
            Point p3 = clipPoints[j];
            Point p4 = clipPoints[(j+1)%clipPoints.size()];

            if(segmentsIntersect(p1,p2,p3,p4)){
                intersectionPointsPresent = true;
                Point interPoint = getintersectPoint(p1,p2,p3,p4);

                interPoint.isIntersection = true;
                interPoint.distPoly = distance(p1,interPoint);
                interPoint.distClip = distance(p3,interPoint);

                polygonIntersections[i].push_back(interPoint);
                clipIntersections[j].push_back(interPoint);
            }
        }
    }

    return intersectionPointsPresent;

}

bool polyListComparator(Point a, Point b){
    return a.distPoly < b.distPoly;
}

bool clipListComparator(Point a, Point b){
    return a.distClip < b.distClip;
}

void generateCombinedLists(){


    for(int i=0;i<polygonPoints.size();i++){
        polygonCombined.push_back(polygonPoints[i]);
        if(polygonIntersections[i].size()>0){
            sort(polygonIntersections[i].begin(),polygonIntersections[i].end(),polyListComparator);
            polygonCombined.insert(polygonCombined.end(),polygonIntersections[i].begin(),polygonIntersections[i].end());
        }
    }

    for(int i=0;i<clipPoints.size();i++){
        clipCombined.push_back(clipPoints[i]);
        if(clipIntersections[i].size()>0){
            sort(clipIntersections[i].begin(),clipIntersections[i].end(),clipListComparator);
            clipCombined.insert(clipCombined.end(),clipIntersections[i].begin(),clipIntersections[i].end());
        }
    }
}



void markInOut(){

    bool toSet = isPointInsideWindow(polygonCombined[0]);
    for(int i=0;i<polygonCombined.size();i++){
        if(polygonCombined[i].isIntersection){
            polygonCombined[i].isInOut = toSet;

            for(int j=0;j<clipCombined.size();j++){
                if(isEqual(polygonCombined[i],clipCombined[j])){
                    polygonCombined[i].isInOut = toSet;
                }
            }

            toSet = !toSet;
        }
    }
}


void getClippedAreas(){

    vector<Point> visited;

    bool onArea = false;

    queue<int> q;
    q.push(0);
    Point p;

    int polyCtr;
    int clipCtr;
    Point starter;

    while(!q.empty()){
        int topper = q.front();
        q.pop();
        if(topper==polygonCombined.size()){
            continue;
        }

        polyCtr = topper;
        p = polygonCombined[polyCtr];
        visited.push_back(p);

        onArea=false;
        
        while(1){
            if(!onArea){

                if(polyCtr==polygonCombined.size()){
                    break;
                }
                polyCtr ++;
                p = polygonCombined[polyCtr];    
                if(inArr(visited,p)!=-1){
                    break;
                }
                
                visited.push_back(p);
                starter = p;

                if(p.isIntersection && p.isInOut){
                    onArea = true;
                    polyCtr++;
                    q.push(polyCtr);
                    polyCtr = -1;
                    for(int j=0;j<clipCombined.size();j++){
                        if(isEqual(clipCombined[j],p)){
                            clipCtr = (j+1)%clipCombined.size();
                            break;
                        }
                    }
                }
            }
            else{
                if(clipCtr!=-1){
                    int index = inArr(visited,clipCombined[clipCtr]);
                    if(index!=-1){
                        vector<Point>newArea;
                        newArea.insert(newArea.begin(),visited.begin()+index,visited.end());
                        areas.push_back(newArea);
                        break;
                    }
                    else{
                        visited.push_back(clipCombined[clipCtr]);
                        int index1 = inArr(polygonCombined,clipCombined[clipCtr]);
                        if(index1!=-1){
                            clipCtr = -1;
                            polyCtr = (index1+1)%polygonCombined.size();

                        }
                        else{
                            clipCtr = (clipCtr+1)%clipCombined.size();
                        }
                    }
                }
                else{
                    int index = inArr(visited,polygonCombined[polyCtr]);
                    if(index!=-1){
                        vector<Point>newArea;
                        newArea.insert(newArea.begin(),visited.begin()+index,visited.end());
                        areas.push_back(newArea);
                        break;
                    }
                    else{
                        visited.push_back(polygonCombined[polyCtr]);

                        if(polygonCombined[polyCtr].isIntersection && polygonCombined[polyCtr].isInOut){
                            clipCtr = (inArr(clipCombined,polygonCombined[polyCtr])+1)%clipCombined.size();
                            polyCtr++;
                            q.push(polyCtr);
                            polyCtr = -1;
                        }
                        else{
                            polyCtr = (polyCtr+1)%polygonCombined.size();
                        }
                    }

                }
            }
        }

    }

}
void weilerAtherton(){

    bool b = generateIntersectionPoints();

    if(b){
        generateCombinedLists();
        markInOut();
        getClippedAreas();
    }
    else{

        if(isPointInsidePolygon(clipPoints[0])){
            vector<Point> newArea;
            newArea.insert(newArea.begin(),clipPoints.begin(),clipPoints.end());
            areas.push_back(newArea);
        }
        else if(isPointInsideWindow(polygonPoints[0])){
            vector<Point> newArea;
            newArea.insert(newArea.begin(),polygonPoints.begin(),polygonPoints.end());
            areas.push_back(newArea);

        }
    }
    complete = true;
    glutPostRedisplay();

}

void init(void) {    
    // Set metrix mode to projection.
    glMatrixMode(GL_PROJECTION);
    
    // Remove any rotations or translations present.
    glLoadIdentity();
    glOrtho(0.0, 500, 500, 0.0, 0.0, 1.0);

    drawPoly = false;
    drawClip = false;
}

// Stores vertices on click.
void mouse_down(int button, int state, int x, int y) {
    if(drawPoly && drawClip){
        return;
    }
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN) {
                Point np;
                if(!drawPoly){
                    np.x = x;
                    np.y = y;
                    polygonPoints.push_back(np);
                    break;
                }
                if(!drawClip){
                    np.x = x;
                    np.y = y;
                    clipPoints.push_back(np);
                    break;
                }
            }
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN) {
                if(!drawPoly){
                    drawPoly = true;
                    break;
                }
                if(!drawClip){
                    drawClip = true;

                    weilerAtherton();
                    break;
                }
            }

    }
    glutPostRedisplay();

}

void display(void) {

    // Clear buffer data.
    glClear(GL_COLOR_BUFFER_BIT);

    if(!complete){
    
    if(drawPoly){
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < polygonPoints.size(); i++) {
            glVertex2f(polygonPoints.at(i).x, polygonPoints.at(i).y);
        }
        glEnd();
    }
    else{
        glPointSize(5);
        glColor3f(1.0, 0.0, 0.0);
        for (int i = 0; i < polygonPoints.size(); i++) {
            glBegin(GL_POINTS);
            glVertex2f(polygonPoints.at(i).x, polygonPoints.at(i).y);
            glEnd();
        }
    }

    if(drawClip){
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < clipPoints.size(); i++) {
            glVertex2f(clipPoints.at(i).x, clipPoints.at(i).y);
        }
        glEnd();
    }
    else{
        glPointSize(5);
        glColor3f(0.0, 1.0, 0.0);
        for (int i = 0; i < clipPoints.size(); i++) {
            glBegin(GL_POINTS);
            glVertex2f(clipPoints.at(i).x, clipPoints.at(i).y);
            glEnd();
        }

    }
    
    }
    else{
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < clipPoints.size(); i++) {
            glVertex2f(clipPoints.at(i).x, clipPoints.at(i).y);
        }
        glEnd();
    
    for(int i=0;i<areas.size();i++){
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0, 1.0, 1.0);
        for (int j = 0; j < areas[i].size(); j++) {
            glVertex2f(areas[i][j].x, areas[i][j].y);
        }
        glEnd();

    }
    }
    // Render the frame to show the results.
    glFlush();
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    
    glutCreateWindow("Assignment 3: Wieler Atherton");
    
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse_down);
    
    glutMainLoop();
    
    return 0;
    
}