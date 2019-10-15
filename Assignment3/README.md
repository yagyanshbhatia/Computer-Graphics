1. click to form vertices
2. Right click to form first polygon (To be clipped)

Repeat above two again, for the next polygon (clipping window)
The intersection is shown in white color. 

<code>NOTE: both polygons must be drawn either clockwise, or both anti-clockwise for desired results.</code>

<code> WARNING ⚠️ : there's no support for the case in which subsequent points are both outside the window, but that edge is still intersecting the clipping window. Take special note to ensure this for first and last points too as an edge is implied between them </code>


use <code>g++ -o graphics scanFill.cpp -framework GLUT -framework OpenGL -Wno-deprecated -std=c++17</code> to compile and <code>./graphics</code> to execute

Thanks [Shaurya Gomber](https://github.com/shaurya31) for the code!
