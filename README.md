DESIGN

>> Cube data structure: 

The 3 x 3 x 3 cube is represented by a 3 x 3 x 3 array, where each array element represents a single component cube. Each array element is defined as a structure, which contains a 8 x 3 array to store the (x,y,z) coordinates of the 8 vertices and a 6 x 3 array to store the colour of the 6 faces of each component cube.

>> Cube initialization:

The original vertex coordinates and face colours are assigned in the function initCube(). The initial cube is centered at (0, 0, 0). It has red for the front face, orange for the back, blue for the left, green for the right, white for the top and yellow for the bottom. The cube is initially viewed from eye position (5, 5, 5) to give a 3-D view of the cube.

>> Rotating whole cube:

When the J, L, I and K buttons are pressed from the keyboard, the program interprets it as a request from the user to rotate the entire cube, left, right, down and up respectively. 

>> How to play:

Commands are case-insensitive

Cube visualization

L: rotate right

J: rotate left

I: rotate down

K: rotate up

+: zoom in

-: zoom out

mouse left click: zoom in

mouse right click: zoom out

Face selection (for moving parts)

There is no face selection feedback. (Consider left-bottom as origin)

Along x axis

Q: select 1st layer

W: select 2nd layer

E: select 3rd layer

Along y axis

A: select 1st layer

S: select 2nd layer

D: select 3rd layer

Along z axis

Z: select 1st layer

X: select 2nd layer

C: select 3rd layer

Face rotation

O: rotate selected face clockwise

U: rotate selected face counter-clockwise
