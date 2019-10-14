1. click to form vertices
2. Right click to form first polygon (To be clipped)
Repeat above two again, for the next polygon (clipping window)
The intersection is shown in white color. 

<code>NOTE: both polygons must be drawn either clockwise, or both anti-clockwise for desired results.</code>


use <code>g++ -o graphics scanFill.cpp -framework GLUT -framework OpenGL -Wno-deprecated -std=c++17</code> to compile and <code>./graphics</code> to execute

Thanks [Shaurya Gomber](https://github.com/shaurya31) for the code!