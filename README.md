# Rubiks_Cube-ComputerGraphics
Implementation:

>	Cube data structure:

The 3 x 3 x 3 cube is represented by a 3 x 3 x 3 array, where each array element represents a single component cube. Each array element is defined as a structure, which contains a 8 x 3 array to store the (x,y,z) coordinates of the 8 vertices and a 6 x 3 array to store the color of the 6 faces of each component cube.


>	Cube initialization:

The original vertex coordinates and face colors are assigned in the function initCube(). The initial cube is centered at (0, 0, 0). It has green for the front face, cyan for the back, red for the left, blue for the right, magenta for the top and yellow for the bottom. All the inside faces are gray. The cube is initially viewed from eye position (5, 5, 5) to give a 3-D view of the cube.

>	Rotating whole cube:

When the left mouse button is clicked outside the cube, the program interprets it as a request from the user to rotate the entire cube. The program then detects the motion vector of the mouse to determine which direction to rotate the cube.

>	Rotating one slice of the cube:

When the left mouse button is clicked on the cube, the program interprets it as a request from the user to rotate one slice of the cube. The program first determines which face of which component cube has been selected by reading the mouse pointing position and performing unprojection to get the original world coordinates. It then determines which of the two possible slices containing that component cube to rotate by detecting the motion vector of the mouse. To do this, we have to unproject the window vector back to the world coordinates. Once the rotating slice is decided, it can not be changed within the same mouse dragging session. However, the direction of rotation is sensitive to the direction of the mouse motion. For any direction, the angle of rotation is incremented by 1 degree with each mouse motion of the same direction. We visualize the rotation by first rendering the 2 stationary slices, then adding the rotation matrix, and rendering the rotating slice. The rotation is stopped when user releases the mouse button. The slice is then automatically repositioned to the closest slot. Since the component cubes (in world coordinate system) are always repositioned to the original coordinates, the vertex coordinate of our cube model does not need to be modified after rotation. However, the face colors of the rotating slice must be updated.

> How to play:

If you press your mouse button and hold it in the black area of the window and drag, the whole cube will rotate according to your mouse motion, but if you pushed the mouse button on one facet of the cube, one slice will twist accordingly.

