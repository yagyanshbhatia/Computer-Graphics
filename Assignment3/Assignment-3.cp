#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <queue>
#include <string.h>
#include <unistd.h>
#include <set>

using namespace std;

class DisplayInterface;
class DisplayGraphics;
class ScanFill;
class DisplayInterface{
    public:
    virtual void Initialise(int argc, char** argv, bool useMouseClick) = 0;
    virtual void DisplayPolygon() = 0;
    virtual void CoordinateInitialise() = 0;
};

class Point{
    public:
    GLint x,y;
    bool isIntersection;
    bool isInOut;
    bool isValid;
    int polygonDistance;
    int clippingDistance;
    Point() : x(0), y(0), isValid(true), isInOut(false), isIntersection(false), polygonDistance(0), clippingDistance(0) {}
    Point(GLint a, GLint b) : x(a), y(b), isValid(true), isInOut(false), isIntersection(false), polygonDistance(0), clippingDistance(0) {}

    GLint GetX() const {
        return x;
    }

    GLint GetY() const {
        return y;
    }

    void IncrementX() {
        x++;
    }

    void IncrementY() {
        y++;
    }

    void DecrementX() {
        x--;
    }

    void DecrementY() {
        y--;
    }

    void Plot(){
        glBegin(GL_POINTS);
        glVertex2i(x , y);
        glEnd();
    }

    void PlotNegative(){
        glBegin(GL_POINTS);
        glVertex2i(-x , -y);
        glEnd();
    }

    void PlotWithOffset(Point &offset){
        glBegin(GL_POINTS);
        glVertex2i(offset.GetX() + x, offset.GetY() + y);
        glEnd();
    }

    void PlotNegativeWithOffset(Point &offset){
        glBegin(GL_POINTS);
        glVertex2i(offset.GetX() - x, offset.GetY() - y);
        glEnd();
    }

    bool isEqual(Point A){
        return (A.x == x) && (A.y == y);
    }

    void TakeInputTerminal(){
        cin >> x >> y;
    }
};

class HelperFunctions{
    public:
    HelperFunctions() {}
    int findInArr(vector<Point> &v, Point p){
        for(int i = 0; i < v.size(); i++){
            if (p.isEqual(v[i])) {
                return i;
            }
        }
        return -1;
    }

    bool IsClockwisePoints(vector<Point> &Points)
    {
            int size = Points.size();
            double sum = 0.0;
            for (int i = 0; i < Points.size(); i++) {
                Point p1 = Points[i];
                Point p2 = Points[(i + 1) % size];
                sum += (p2.x - p1.x) * (p2.y + p1.y);
            }
            return sum > 0.0;
    }

    int cross(Point& p0, Point& p1, Point& p2) {
        return ((p2.x - p0.x) * (p1.y - p0.y) - (p1.x - p0.x) * (p2.y - p0.y));
    }

    bool isPointInsidePolygon(vector<Point> &clipPoints, Point p) {
        int cnt = 0, size = clipPoints.size();
        for (int i = 0; i < size; i++) {
            Point p1 = clipPoints[i];
            Point p2 = clipPoints[(i + 1) % size];
            if (p1.y == p2.y) continue;
            if (p.y < min(p1.y, p2.y)) continue;
            if (p.y >= max(p1.y, p2.y)) continue;
            if (cross(p1, p, p2)*(p2.y - p1.y) > 0) {
                cnt++;
            }
        }
        return (cnt%2 == 1);
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

        if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) return true;
        int D = (p2.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p2.y - p1.y);
        if (D == 0) return false;
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
        if(D == 0) p.isValid=false;
        int D1 = b2 * (p2.x - p1.x) - b1 * (p4.x - p3.x);
        int D2 = b2 * (p2.y - p1.y) - b1 * (p4.y - p3.y);
        p.x = D1 / D;
        p.y = D2 / D;
        return p;
    }

    int distance(Point p1,Point p2) {
        return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
    }

    static bool polyListComparator(Point a, Point b){
        return a.polygonDistance < b.polygonDistance;
    }

    static bool clipListComparator(Point a, Point b){
        return a.clippingDistance < b.clippingDistance;
    }
};

class Polygon{
	public:
		vector<Point> pts;
        HelperFunctions helper;
        int size;
        void takeTerminalInput(){
            cout << "Enter number of vertices. : " ;
            cin >> size;
            pts = vector<Point>(size);
            cout << "Now enter each vertex coordinates in order(clockwise or anti-clockwise). " << endl;
            for(int i=0;i<size;i++){
                pts[i].TakeInputTerminal();
            }
        }

        void print(){
            for (Point p : pts){
                cout << p.x << " " << p.y << endl;
            }
        }

        void alignAndPrint(){
            cout << "Before alignment" << endl;
            print();
            
            // whether to reverse or not.
            bool rotation = helper.IsClockwisePoints(pts);
            // cout << val << endl;
            if(rotation) {
                reverse(pts.begin(), pts.end());
            }

            cout << "After alignment" << endl;
            print();
        }

		void drawPolygonLine(){
            size = pts.size();
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < size; i++) {
                glVertex2i(pts[i].x, pts[i].y);
            }
            glEnd();
        }

        void drawPolygonPoints(){
            size = pts.size();
            for (int i = 0; i < size; i++) {
                pts[i].Plot();
            }
        }
};

int WINDOW_ID;

class WeilerAtherton{
    public:
    vector<Point> polygonPoints;
    vector<Point> clipPoints;

    vector<Point> polygonIntersections[100];
    vector<Point> clipIntersections[100];

    vector<Point> polygonCombined;
    vector<Point> clipCombined;

    vector<Polygon> areas;
    bool isComplete;
    HelperFunctions helper;

    WeilerAtherton() {
        isComplete = false;
    }

    void updateArrays(vector<Point> polygonPoints, vector<Point> clipPoints){
        this->polygonPoints = polygonPoints;
        this->clipPoints = clipPoints;
    }

    bool generateIntersectionPoints(){

        bool intersectionPointsPresent = false;

        for(int i = 0; i < polygonPoints.size(); i++){
            Point p1 = polygonPoints[i];
            Point p2 = polygonPoints[(i+1)%polygonPoints.size()];

            for(int j = 0; j < clipPoints.size(); j++){
                Point p3 = clipPoints[j];
                Point p4 = clipPoints[(j+1)%clipPoints.size()];

                if(helper.segmentsIntersect(p1,p2,p3,p4)){
                    intersectionPointsPresent = true;
                    Point interPoint = helper.getintersectPoint(p1,p2,p3,p4);

                    interPoint.isIntersection = true;
                    interPoint.polygonDistance = helper.distance(p1,interPoint);
                    interPoint.clippingDistance = helper.distance(p3,interPoint);

                    polygonIntersections[i].push_back(interPoint);
                    clipIntersections[j].push_back(interPoint);
                }
            }
        }

        return intersectionPointsPresent;
    }

    void generateCombinedLists(){
        for(int i=0;i<polygonPoints.size();i++){
            polygonCombined.push_back(polygonPoints[i]);
            if(polygonIntersections[i].size()>0){
                sort(polygonIntersections[i].begin(),polygonIntersections[i].end(), helper.polyListComparator);
                polygonCombined.insert(polygonCombined.end(),polygonIntersections[i].begin(),polygonIntersections[i].end());
            }
        }

        for(int i=0;i<clipPoints.size();i++){
            clipCombined.push_back(clipPoints[i]);
            if(clipIntersections[i].size()>0){
                sort(clipIntersections[i].begin(),clipIntersections[i].end(), helper.clipListComparator);
                clipCombined.insert(clipCombined.end(),clipIntersections[i].begin(),clipIntersections[i].end());
            }
        }

        cout << "Combined Polygon\n";
        for(Point p : polygonCombined){
            cout << p.x << " " << p.y << endl;
        }
        cout << "Combined Window\n";
        for(Point p : clipCombined){
            cout << p.x << " " << p.y << endl;
        }
    }

    void markInOut(){
        bool toSet = helper.isPointInsidePolygon(clipPoints, polygonCombined[0]);
        for(int i=0;i<polygonCombined.size();i++){
            if(polygonCombined[i].isIntersection){
                polygonCombined[i].isInOut = toSet;

                for(int j=0;j<clipCombined.size();j++){
                    if(polygonCombined[i].isEqual(clipCombined[j])){
                        clipCombined[j].isInOut = toSet;
                    }
                }
                toSet = !toSet;
            }
        }
    }

    void getClippedAreas(){
        int it=0, it1=0;
        Polygon gblPolygon;
        set<int> entryIndexes;
        int polygonSize = polygonCombined.size();
        int windowSize = clipCombined.size();
        gblPolygon.pts.clear();
        for(it = 0; it < polygonSize; it++){
            if(polygonCombined[it].isIntersection && polygonCombined[it].isInOut==false) entryIndexes.insert(it);
        }

        do{
            it = *(entryIndexes.begin());
            entryIndexes.erase(it);
            while(it < polygonSize){
                cout << "it__0 = " << it << endl;
                gblPolygon.pts.push_back(polygonCombined[it]);
                while(1){
                    it = (it+1)%polygonSize;
                    if(polygonCombined[it].isIntersection && polygonCombined[it].isInOut) break;
                    if(polygonCombined[it].isIntersection && polygonCombined[it].isInOut==false) entryIndexes.erase(it);
                    gblPolygon.pts.push_back(polygonCombined[it]);
                }

                cout << "it__1 = " << it << endl;
                it1 = helper.findInArr(clipCombined, polygonCombined[it]);
                if(it1 == -1) exit(0);
                while(1){
                    if(clipCombined[it1].isIntersection && clipCombined[it1].isInOut == false) break;
                    gblPolygon.pts.push_back(clipCombined[it1]);
                    it1=(it1+1)%windowSize;
                }

                cout << "it__2 = " << it << endl;
                if(gblPolygon.pts[0].isEqual(clipCombined[it1])){
                    cout << "Area found" << endl;
                    Polygon newPolygon;
                    newPolygon.pts.clear();
                    newPolygon.pts = gblPolygon.pts;
                    gblPolygon.pts.clear();
                    areas.push_back(newPolygon);
                    break;
                }
                
                cout << "it__3 = " << it << endl;
                for(; it < polygonSize; it++){
                    if(polygonCombined[it].isEqual(clipCombined[it1])) {
                        break;
                    };
                }

                cout << "it__4 = " << it << endl;
            }
        }
        while(!entryIndexes.empty());
    }

    void run(){
        bool intersectionExists = generateIntersectionPoints();
        if(intersectionExists){
            generateCombinedLists();
            markInOut();
            getClippedAreas();
        }
        else{
            if(helper.isPointInsidePolygon(polygonPoints, clipPoints[0])){
                Polygon newArea;
                newArea.pts.insert(newArea.pts.begin(),clipPoints.begin(),clipPoints.end());
                areas.push_back(newArea);
            }
            else if(helper.isPointInsidePolygon(clipPoints, polygonPoints[0])){
                Polygon newArea;
                newArea.pts.insert(newArea.pts.begin(),polygonPoints.begin(),polygonPoints.end());
                areas.push_back(newArea);
            }
        }
        isComplete = true;
        glutPostRedisplay();
    }
};

bool drawPoly;
bool drawClip;
Polygon polygon, window;
WeilerAtherton *runInstance;
bool isPrinted;
class DisplayGraphics : DisplayInterface {
    private:
    bool useMouseClick;
    
    static void mouseFunction(int button, int state, int x, int y) {
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
                        polygon.pts.push_back(np);
                        break;
                    }
                    if(!drawClip){
                        np.x = x;
                        np.y = y;
                        window.pts.push_back(np);
                        break;
                    }
                }
            case GLUT_RIGHT_BUTTON:
                if(state == GLUT_DOWN) {
                    if(!drawPoly){
                        drawPoly = true;
                        polygon.alignAndPrint();
                        break;
                    }
                    if(!drawClip){
                        drawClip = true;
                        window.alignAndPrint();
                        
                        runInstance->updateArrays(polygon.pts, window.pts);
                        runInstance->run();
                        break;
                    }
                }

        }
        glutPostRedisplay();

    }


    // Esc to exit.
    static void keyboard(unsigned char key, int xmouse, int ymouse) {
        switch (key) {
            case 27:
                glutDestroyWindow(WINDOW_ID);
                exit(0);
                break;
        }
        glutPostRedisplay();
    }

    static void displayFunction() {
        glClear(GL_COLOR_BUFFER_BIT);
        if(runInstance->isComplete == false){
            if(drawPoly){
                glColor3f(1.0, 0.0, 0.0);
                polygon.drawPolygonLine();
            }
            else{
                glPointSize(5);
                glColor3f(1.0, 0.0, 0.0);
                polygon.drawPolygonPoints();
            }

            if(drawClip){
                glColor3f(0.0, 1.0, 0.0);
                window.drawPolygonLine();
            }
            else{
                glPointSize(5);
                glColor3f(0.0, 1.0, 0.0);
                window.drawPolygonPoints();
            }
        }
        else{
            glColor3f(0.0, 1.0, 0.0);
            window.drawPolygonLine();
            glColor3f(1.0, 1.0, 1.0);
            for(int i = 0; i < runInstance->areas.size();i++){
                if(!isPrinted){
                    cout << "Area-" << i << endl;
                    runInstance->areas[i].print();
                    cout << endl;
                }
                runInstance->areas[i].drawPolygonLine();
            }
            isPrinted = true;

        }
        // Render the frame to show the results.
        glFlush();
    }

    void takeTerminalInput(){
        polygon.takeTerminalInput();
        window.takeTerminalInput();
        polygon.alignAndPrint();
        window.alignAndPrint();
                        
        runInstance->updateArrays(polygon.pts, window.pts);
        runInstance->run();
        drawClip = drawPoly = true;
    }

    public:
    DisplayGraphics() {
        runInstance = new WeilerAtherton();
    }

    void Initialise(int argc, char** argv, bool useMouseClick) {
        glutInit (&argc, argv);
        glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
        glutInitWindowSize (500, 500);
        glutInitWindowPosition (100, 100);
        WINDOW_ID = glutCreateWindow ("Programming Assignment-3 : Weiler-Atherton Polygon Clipping.");
        this->useMouseClick = useMouseClick;
    }

    void DisplayPolygon(){
        glutDisplayFunc(displayFunction);
        glutKeyboardFunc(keyboard);

        if(useMouseClick){
            glutMouseFunc(mouseFunction);
        }
        else{
            takeTerminalInput();
        }
        glutMainLoop ();
    }

    void CoordinateInitialise(){
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        glOrtho (0.0, 500, 500.0, 0.0, 0.0, 1.0);
        drawClip = drawPoly = false;
    }  

};

int main(int argc, char** argv){
    int response = 0;
    DisplayGraphics *graphics = new DisplayGraphics();
    cout << "Mode of input mouse-click(0, default) or terminal(1)?" << endl;
    cin >> response;
    isPrinted = false;
    graphics->Initialise(argc, argv, response==0);
    graphics->CoordinateInitialise();
    graphics->DisplayPolygon();
    return 0;
}