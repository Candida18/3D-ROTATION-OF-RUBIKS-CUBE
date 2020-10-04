
/*
 * This program create a window 400 x 400 pixels and draws a Rubik's cube.
 *
 * To terminate the program, press the escape key.  The mouse pointer must
 * be over the window.
 */
/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define CUBEDIM 3        /* 3 x 3 Rubik's cube */
#define CUBESIZE 1.0     /* size of each component cube */
#define CUBEINTERVAL 0.2  /* space between each component cube */

/* indices for the six faces of each component cube */
#define BACK 0
#define FRONT 1
#define RIGHT 2
#define LEFT 3
#define BOTTOM 4
#define TOP 5

#define SPIN_UP 0
#define SPIN_DOWN 1
#define SPIN_LEFT 2
#define SPIN_RIGHT 3

#define ROTATED_90 1
#define ROTATED_180 2
#define ROTATED_270 3

  /* six colors for the six faces of the cube */
GLfloat red[] = {1.0, 0.0, 0.0};
GLfloat green[] = {0.0, 1.0, 0.0};
GLfloat blue[] = {0.0, 0.0, 1.0};
GLfloat cyan[] = {0.0, 1.0, 1.0};
GLfloat yellow[] = {1.0, 1.0, 0.0};
GLfloat magenta[] = {1.0, 0.0, 1.0};
GLfloat gray[] = {0.7, 0.7, 0.7};


  /* define CUBE_VERTEX_TYPE to hold the coordinates (x,y,z) for the
   * 8 vertices of a component cube; and CUBE_FACE_COLOR_TYPE to
   * hold the color values for the 6 faces of each component cube.
   */
typedef GLfloat CUBE_VERTEX_TYPE[8][3];
typedef GLfloat CUBE_FACE_COLOR_TYPE[6][3];

  /* define a 3 x 3 x 3 Rubik's cube */
struct cubetype {
  CUBE_VERTEX_TYPE vertex;
  CUBE_FACE_COLOR_TYPE facecolor;
};
typedef struct cubetype CUBE_TYPE;

CUBE_TYPE RubiksCube[CUBEDIM][CUBEDIM][CUBEDIM];

void drawOneSlice(char rotateAxis, GLint sliceNum, GLenum mode);

/**********************************************************
 *  assign vertex coordinates to (x, y, z)
 *********************************************************/
void assignVertex(GLfloat vertex[3], GLfloat x, GLfloat y, GLfloat z)
{
  vertex[0] = x;
  vertex[1] = y;
  vertex[2] = z;
}

/**********************************************************
 *  copy color b to color a
 *********************************************************/
void copyColor(GLfloat colora[3], GLfloat colorb[])
{
  colora[0] = colorb[0];
  colora[1] = colorb[1];
  colora[2] = colorb[2];
}

/***************************************************************/
/* checks if a point (x, y, z) is inside the cube defined by   */
/* the input argument, vertices.			       */
/* (x, y, z) is in world coordinates.			       */
/***************************************************************/

int is_point_inside_cube(GLfloat x, GLfloat y, GLfloat z,
			 CUBE_VERTEX_TYPE vertices) {
  GLfloat left, right, bottom, top, front, back;

  left = vertices[0][0];
  right = vertices[1][0];
  bottom = vertices[0][1];
  top = vertices[3][1];
  front = vertices[4][2];
  back = vertices[0][2];

  if (((x>left && x<right) || (fabs(x-left)<=0.05 || fabs(x-right)<=0.05)) &&
      ((y>bottom && y<top) || (fabs(y-bottom)<=0.05 || fabs(y-top)<=0.05)) &&
      ((z>back && z<front) || (fabs(z-front)<=0.05 || fabs(z-back)<=0.05)))
    return 1;
  else
    return 0;
}

/***************************************************************/
/* determines which cube face the point (x, y, z) lies in.     */
/* (x, y, z) is in world coordinates.			       */
/***************************************************************/

void face_of_cube(GLfloat x, GLfloat y, GLfloat z, GLint *face) {
  if (x >= 1.68 && x <= 1.72)
    *face = RIGHT;
  else if (x >= -1.72 && x <= -1.68)
    *face = LEFT;
  else if (y >= 1.68 && y <= 1.72)
    *face = TOP;
  else if (y >= -1.72 && y <= -1.68)
    *face = BOTTOM;
  else if (z >= 1.68 && z <= 1.72)
    *face = FRONT;
  else if (z >= -1.72 && z <= -1.68)
    *face = BACK;
  return;
}

/***************************************************************/
/* determines i, j, k (row, column, height) of the cube        */
/* that point (x, y, z) is in.  If (x, y, z) is not inside a   */
/* cube, i, j, k are all set to -1.			       */
/* (x, y, z) is in world coordinates.			       */
/***************************************************************/

void ijk_of_cube(GLfloat x, GLfloat y, GLfloat z,
		 GLint *ii, GLint *jj, GLint *kk) {
  GLint i, j, k;
  int is_inside;

  for (i=0; i<CUBEDIM; ++i) {
    for (j=0; j<CUBEDIM; ++j) {
      for (k=0; k<CUBEDIM; ++k) {
	if (is_point_inside_cube(x, y, z, RubiksCube[i][j][k].vertex)) {
	  *ii = i;
	  *jj = j;
	  *kk = k;
	  return;
        }
      }
    }
  }
  *ii = *jj = *kk = -1;
  return;
}

/***************************************************************/
/* determines face and i, j, k of the cube that point 	       */
/* (x, y, z) is inside.					       */
/* (x, y, z) is in world coordinates.			       */
/***************************************************************/

void get_face_and_ijk_of_cube(GLfloat x, GLfloat y, GLfloat z, GLint *face,
		   GLint *i, GLint *j, GLint *k) {
  face_of_cube(x, y, z, face);
  ijk_of_cube(x, y, z, i, j, k);
}

/***************************************************************/
/* calculates the spin direction based on the mouse motion.    */
/***************************************************************/

void get_slice_spin_direction(GLfloat clickx, GLfloat clicky, GLfloat clickz,
			      GLfloat x, GLfloat y, GLfloat z, GLint face,
			      GLint *dir) {
  GLfloat movex = x - clickx;
  GLfloat movey = y - clicky;
  GLfloat movez = z - clickz;

  switch (face) {
  case BACK: ;
  case FRONT:
    if (fabsf(movex) > fabsf(movey)) {
      if (movex < 0.0 && face == BACK && (*dir == -1 || *dir == SPIN_LEFT))
        *dir = SPIN_RIGHT;
      else if (movex >= 0.0 && face == BACK && (*dir==-1 || *dir==SPIN_RIGHT))
        *dir = SPIN_LEFT;
      else if (movex < 0.0 && face == FRONT && (*dir==-1 || *dir==SPIN_RIGHT))
        *dir = SPIN_LEFT;
      else if (movex >= 0.0 && face == FRONT && (*dir==-1 || *dir==SPIN_LEFT))
        *dir = SPIN_RIGHT;
    }
    else {
      if (movey < 0.0 && (*dir==-1 || *dir==SPIN_UP))
	*dir = SPIN_DOWN;
      else if (movey >= 0.0 && (*dir==-1 || *dir==SPIN_DOWN))
	*dir = SPIN_UP;
    }
    break;
  case RIGHT: ;
  case LEFT:
    if (fabsf(movey) > fabsf(movez)) {
      if (movey < 0.0 && (*dir==-1 || *dir==SPIN_UP))
        *dir = SPIN_DOWN;
      else if (movey >= 0.0 && (*dir==-1 || *dir==SPIN_DOWN))
        *dir = SPIN_UP;
    }
    else {
      if (movez < 0.0 && face == RIGHT && (*dir==-1 || *dir==SPIN_LEFT))
	*dir = SPIN_RIGHT;
      else if (movez >= 0.0 && face == RIGHT && (*dir==-1 || *dir==SPIN_RIGHT))
	*dir = SPIN_LEFT;
      else if (movez < 0.0 && face == LEFT && (*dir==-1 || *dir==SPIN_RIGHT))
	*dir = SPIN_LEFT;
      else if (movez >= 0.0 && face == LEFT && (*dir==-1 || *dir==SPIN_LEFT))
	*dir = SPIN_RIGHT;
    }
    break;
  case BOTTOM: ;
  case TOP:
    if (fabsf(movex) > fabsf(movez)) {
      if (movex < 0.0 && (*dir==-1 || *dir==SPIN_RIGHT))
	*dir = SPIN_LEFT;
      else if (movex >= 0.0 && (*dir==-1 || *dir==SPIN_LEFT))
	*dir = SPIN_RIGHT;
    }
    else {
      if (movez < 0.0 && face == BOTTOM && (*dir==-1 || *dir==SPIN_UP))
	*dir = SPIN_DOWN;
      else if (movez >= 0.0 && face == BOTTOM && (*dir==-1 || *dir==SPIN_DOWN))
	*dir = SPIN_UP;
      else if (movez < 0.0 && face == TOP && (*dir==-1 || *dir==SPIN_DOWN))
        *dir = SPIN_UP;
      else if (movez >= 0.0 && face == TOP && (*dir==-1 || *dir==SPIN_UP))
        *dir = SPIN_DOWN;
    }
    break;
  default:
    break;
  }
}

/***************************************************************/
/* calculates the next angle for the spinning slice.           */
/***************************************************************/

void get_slice_spin_angle(GLint face, GLint spin_dir, GLfloat *spin) {
  switch (face) {
  case BACK:
    if (spin_dir == SPIN_RIGHT || spin_dir == SPIN_UP)
      *spin += 1.0;
    else if (spin_dir == SPIN_LEFT || spin_dir == SPIN_DOWN)
      *spin -= 1.0;
    break;
  case FRONT:
    if (spin_dir == SPIN_RIGHT || spin_dir == SPIN_DOWN)
      *spin += 1.0;
    else if (spin_dir == SPIN_LEFT || spin_dir == SPIN_UP)
      *spin -= 1.0;
    break;
  case RIGHT:
    if (spin_dir == SPIN_RIGHT || spin_dir == SPIN_UP)
      *spin += 1.0;
    else if (spin_dir == SPIN_LEFT || spin_dir == SPIN_DOWN)
      *spin -= 1.0;
    break;
  case LEFT:
    if (spin_dir == SPIN_RIGHT || spin_dir == SPIN_DOWN)
      *spin += 1.0;
    else if (spin_dir == SPIN_LEFT || spin_dir == SPIN_UP)
      *spin -= 1.0;
    break;
  case BOTTOM:
    if (spin_dir == SPIN_RIGHT || spin_dir == SPIN_DOWN)
      *spin += 1.0;
    else if (spin_dir == SPIN_LEFT || spin_dir == SPIN_UP)
      *spin -= 1.0;
    break;
  case TOP:
    if (spin_dir == SPIN_LEFT || spin_dir == SPIN_DOWN)
      *spin += 1.0;
    else if (spin_dir == SPIN_RIGHT || spin_dir == SPIN_UP)
      *spin -= 1.0;
    break;
  default:
    break;
  }
}

/***************************************************************/
/* determines the rotation axis of a slice.		       */
/***************************************************************/

void get_slice_spin_axis(GLint face, GLint spin_dir, char *axis) {
  switch (face) {
  case BACK: ;
  case FRONT:
    if (spin_dir == SPIN_LEFT || spin_dir == SPIN_RIGHT)
      *axis = 'y';
    else if (spin_dir == SPIN_UP || spin_dir == SPIN_DOWN)
      *axis = 'x';
    break;
  case RIGHT: ;
  case LEFT:
    if (spin_dir == SPIN_LEFT || spin_dir == SPIN_RIGHT)
      *axis = 'y';
    else if (spin_dir == SPIN_UP || spin_dir == SPIN_DOWN)
      *axis = 'z';
    break;
  case BOTTOM: ;
  case TOP:
    if (spin_dir == SPIN_LEFT || spin_dir == SPIN_RIGHT)
      *axis = 'z';
    else if (spin_dir == SPIN_UP || spin_dir == SPIN_DOWN)
      *axis = 'x';
    break;
  default:
    break;
  }
}

/***************************************************************/
/* draws slices of a cube.				       */
/***************************************************************/

void draw_slices(GLfloat spin, char axis, GLint i, GLint j, GLint k) {
  GLfloat axis_x = 0.0, axis_y = 0.0, axis_z = 0.0;
  GLint slice_no, index;

  switch (axis) {
  case 'x': ;
  case 'X':
    axis_x = 1.0;
    slice_no = i;
    break;
  case 'y': ;
  case 'Y':
    axis_y = 1.0;
    slice_no = j;
    break;
  case 'z': ;
  case 'Z':
    axis_z = 1.0;
    slice_no = k;
    break;
  default:
    break;
  }

  for (index = 0; index < CUBEDIM; index++)
    if (index != slice_no)
      drawOneSlice(axis, index, GL_RENDER);

  glRotatef(spin, axis_x, axis_y, axis_z);
  drawOneSlice(axis, slice_no, GL_RENDER);
}

/***************************************************************/
/* changes face colors of a slice.			       */
/***************************************************************/

void change_slice_color(char axis, GLint slice_no) {
  GLint c, f;
  GLfloat color[3][6][3];

  switch (axis) {
  case 'x': ;
  case 'X':
    for (c=0; c<CUBEDIM; c++)
      for (f=0; f<6; f++)
        copyColor(color[c][f],
			RubiksCube[slice_no][c][CUBEDIM-1].facecolor[f]);
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[slice_no][c][CUBEDIM-1].facecolor[FRONT],
	RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[TOP]);
      copyColor(RubiksCube[slice_no][c][CUBEDIM-1].facecolor[TOP],
                RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[BACK]);
      copyColor(RubiksCube[slice_no][c][CUBEDIM-1].facecolor[BACK],
                RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[BOTTOM]);
      copyColor(RubiksCube[slice_no][c][CUBEDIM-1].facecolor[BOTTOM],
                RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[FRONT]);
      copyColor(RubiksCube[slice_no][c][CUBEDIM-1].facecolor[LEFT],
                RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[LEFT]);
      copyColor(RubiksCube[slice_no][c][CUBEDIM-1].facecolor[RIGHT],
                RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[RIGHT]);
    }
    for (c=0; c<CUBEDIM; c++)  {
      copyColor(RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[TOP],
                RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[BACK]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[BACK],
                RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[BOTTOM]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[BOTTOM],
                RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[FRONT]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[FRONT],
                RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[TOP]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[LEFT],
                RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[LEFT]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1][CUBEDIM-1-c].facecolor[RIGHT],
                RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[RIGHT]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[FRONT],
                RubiksCube[slice_no][0][c].facecolor[TOP]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[TOP],
                RubiksCube[slice_no][0][c].facecolor[BACK]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[BACK],
                RubiksCube[slice_no][0][c].facecolor[BOTTOM]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[BOTTOM],
                RubiksCube[slice_no][0][c].facecolor[FRONT]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[LEFT],
                RubiksCube[slice_no][0][c].facecolor[LEFT]);
      copyColor(RubiksCube[slice_no][CUBEDIM-1-c][0].facecolor[RIGHT],
                RubiksCube[slice_no][0][c].facecolor[RIGHT]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[slice_no][0][c].facecolor[FRONT], color[c][TOP]);
      copyColor(RubiksCube[slice_no][0][c].facecolor[TOP], color[c][BACK]);
      copyColor(RubiksCube[slice_no][0][c].facecolor[BACK],color[c][BOTTOM]);
      copyColor(RubiksCube[slice_no][0][c].facecolor[BOTTOM],color[c][FRONT]);
      copyColor(RubiksCube[slice_no][0][c].facecolor[LEFT],color[c][LEFT]);
      copyColor(RubiksCube[slice_no][0][c].facecolor[RIGHT],color[c][RIGHT]);
    }
    break;
  case 'y': ;
  case 'Y':
    for (c=0; c<CUBEDIM; c++)
      for (f=0; f<6; f++)
        copyColor(color[c][f],
                        RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[f]);
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[FRONT],
                RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[LEFT]);
      copyColor(RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[LEFT],
                RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[BACK]);
      copyColor(RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[BACK],
                RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[RIGHT]);
      copyColor(RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[RIGHT],
                RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[FRONT]);
      copyColor(RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[TOP],
                RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[TOP]);
      copyColor(RubiksCube[CUBEDIM-1-c][slice_no][CUBEDIM-1].facecolor[BOTTOM],
                RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[BOTTOM]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[FRONT],
                RubiksCube[c][slice_no][0].facecolor[LEFT]);
      copyColor(RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[LEFT],
                RubiksCube[c][slice_no][0].facecolor[BACK]);
      copyColor(RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[BACK],
                RubiksCube[c][slice_no][0].facecolor[RIGHT]);
      copyColor(RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[RIGHT],
                RubiksCube[c][slice_no][0].facecolor[FRONT]);
      copyColor(RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[TOP],
                RubiksCube[c][slice_no][0].facecolor[TOP]);
      copyColor(RubiksCube[0][slice_no][CUBEDIM-1-c].facecolor[BOTTOM],
                RubiksCube[c][slice_no][0].facecolor[BOTTOM]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[c][slice_no][0].facecolor[FRONT],
                RubiksCube[CUBEDIM-1][slice_no][c].facecolor[LEFT]);
      copyColor(RubiksCube[c][slice_no][0].facecolor[LEFT],
                RubiksCube[CUBEDIM-1][slice_no][c].facecolor[BACK]);
      copyColor(RubiksCube[c][slice_no][0].facecolor[BACK],
                RubiksCube[CUBEDIM-1][slice_no][c].facecolor[RIGHT]);
      copyColor(RubiksCube[c][slice_no][0].facecolor[RIGHT],
                RubiksCube[CUBEDIM-1][slice_no][c].facecolor[FRONT]);
      copyColor(RubiksCube[c][slice_no][0].facecolor[TOP],
                RubiksCube[CUBEDIM-1][slice_no][c].facecolor[TOP]);
      copyColor(RubiksCube[c][slice_no][0].facecolor[BOTTOM],
                RubiksCube[CUBEDIM-1][slice_no][c].facecolor[BOTTOM]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[CUBEDIM-1][slice_no][c].facecolor[FRONT],
                color[c][LEFT]);
      copyColor(RubiksCube[CUBEDIM-1][slice_no][c].facecolor[LEFT],
                color[c][BACK]);
      copyColor(RubiksCube[CUBEDIM-1][slice_no][c].facecolor[BACK],
                color[c][RIGHT]);
      copyColor(RubiksCube[CUBEDIM-1][slice_no][c].facecolor[RIGHT],
                color[c][FRONT]);
      copyColor(RubiksCube[CUBEDIM-1][slice_no][c].facecolor[TOP],
                color[c][TOP]);
      copyColor(RubiksCube[CUBEDIM-1][slice_no][c].facecolor[BOTTOM],
                color[c][BOTTOM]);
    }
    break;
  case 'z': ;
  case 'Z':
    for (c=0; c<CUBEDIM; c++)
      for (f=0; f<6; f++)
        copyColor(color[c][f],
                        RubiksCube[c][CUBEDIM-1][slice_no].facecolor[f]);
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[c][CUBEDIM-1][slice_no].facecolor[TOP],
                RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[RIGHT]);
      copyColor(RubiksCube[c][CUBEDIM-1][slice_no].facecolor[RIGHT],
                RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[BOTTOM]);
      copyColor(RubiksCube[c][CUBEDIM-1][slice_no].facecolor[BOTTOM],
                RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[LEFT]);
      copyColor(RubiksCube[c][CUBEDIM-1][slice_no].facecolor[LEFT],
                RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[TOP]);
      copyColor(RubiksCube[c][CUBEDIM-1][slice_no].facecolor[FRONT],
                RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[FRONT]);
      copyColor(RubiksCube[c][CUBEDIM-1][slice_no].facecolor[BACK],
                RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[BACK]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[TOP],
                RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[RIGHT]);
      copyColor(RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[RIGHT],
                RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[BOTTOM]);
      copyColor(RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[BOTTOM],
                RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[LEFT]);
      copyColor(RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[LEFT],
                RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[TOP]);
      copyColor(RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[FRONT],
                RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[FRONT]);
      copyColor(RubiksCube[CUBEDIM-1][CUBEDIM-1-c][slice_no].facecolor[BACK],
                RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[BACK]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[TOP],
                RubiksCube[0][c][slice_no].facecolor[RIGHT]);
      copyColor(RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[RIGHT],
                RubiksCube[0][c][slice_no].facecolor[BOTTOM]);
      copyColor(RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[BOTTOM],
                RubiksCube[0][c][slice_no].facecolor[LEFT]);
      copyColor(RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[LEFT],
                RubiksCube[0][c][slice_no].facecolor[TOP]);
      copyColor(RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[FRONT],
                RubiksCube[0][c][slice_no].facecolor[FRONT]);
      copyColor(RubiksCube[CUBEDIM-1-c][0][slice_no].facecolor[BACK],
                RubiksCube[0][c][slice_no].facecolor[BACK]);
    }
    for (c=0; c<CUBEDIM; c++) {
      copyColor(RubiksCube[0][c][slice_no].facecolor[TOP],
                color[c][RIGHT]);
      copyColor(RubiksCube[0][c][slice_no].facecolor[RIGHT],
                color[c][BOTTOM]);
      copyColor(RubiksCube[0][c][slice_no].facecolor[BOTTOM],
                color[c][LEFT]);
      copyColor(RubiksCube[0][c][slice_no].facecolor[LEFT],
                color[c][TOP]);
      copyColor(RubiksCube[0][c][slice_no].facecolor[FRONT],
                color[c][FRONT]);
      copyColor(RubiksCube[0][c][slice_no].facecolor[BACK],
                color[c][BACK]);
    }
  }
}

/***************************************************************/
/* updates colors of a slice.				       */
/***************************************************************/

void update_slice(GLint face, char axis, GLint i, GLint j, GLint k,
		  GLfloat spin) {
  GLint slice_no;

  switch (axis) {
  case 'x': ;
  case 'X':
    slice_no = i;
    break;
  case 'y': ;
  case 'Y':
    slice_no = j;
    break;
  case 'z': ;
  case 'Z':
    slice_no = k;
    break;
  default:
    break;
  }

  while (spin > 45.0 && spin <= 315.0) {
    change_slice_color(axis, slice_no);
    spin -= 90.0;
  }
}

/***************************************************************/
/* scrambles a rubik's cube.				       */
/***************************************************************/

void scramble_rubik_cube() {
  GLint index, rotation_count;
  char axis;
  GLint axis_no, slice_no;

  for (index = 0; index < 10; index++) {
    axis_no = rand() % 3;
    switch (axis_no) {
    case 0:
      axis = 'x';
      break;
    case 1:
      axis = 'y';
      break;
    case 2:
      axis = 'z';
      break;
    }
    slice_no = rand() % 3;
    rotation_count = rand() % 4;
    for (; rotation_count > 0; rotation_count--)
      change_slice_color(axis, slice_no);
  }
}

/**********************************************************************
   initialize the vertex coordinates and face colors for the cube
**********************************************************************/
void initCube(void)
{
  GLint i, j, k;
  GLfloat x, y, z;
  GLfloat color_back[3], color_front[3], color_right[3], color_left[3],
          color_bottom[3], color_top[3];
  GLfloat size = CUBESIZE+CUBEINTERVAL;
  GLfloat shift = 0.5*(CUBEDIM*size-CUBEINTERVAL);

  for (i=0; i<CUBEDIM; ++i) {
    x = i * size - shift;

      /* assign original face colors */
    if (i == 0) {  /* leftmost slice */
      copyColor(color_left, red);
      if ( i != CUBEDIM-1 )
        copyColor(color_right, gray);
      else
        copyColor(color_right, blue);
    }
    else if (i == CUBEDIM-1) {  /* rightmost slice */
      copyColor(color_left, gray);
      copyColor(color_right, blue);
    }
    else {  /* slices in the middle */
      copyColor(color_left, gray);
      copyColor(color_right, gray);
    }

    for (j=0; j<CUBEDIM; ++j) {
      y = j * size - shift;

        /* assign original face colors */
      if (j == 0) {  /* bottommost slice */
        copyColor(color_bottom, yellow);
        if ( j != CUBEDIM-1 )
          copyColor(color_top, gray);
        else
          copyColor(color_top, magenta);
      }
      else if (j == CUBEDIM-1) {  /* topmost slice */
        copyColor(color_bottom, gray);
        copyColor(color_top, magenta);
      }
      else {  /* slices in the middle */
        copyColor(color_bottom, gray);
        copyColor(color_top, gray);
      }

      for (k=0; k<CUBEDIM; ++k) {
        z = k * size - shift;

        if (k == 0) {
          copyColor(color_back, cyan);
          if (k != CUBEDIM-1)
            copyColor(color_front, gray);
          else
            copyColor(color_front, green);
        }
        else if (k == CUBEDIM-1) {
          copyColor(color_back, gray);
          copyColor(color_front, green);
        }
        else {
          copyColor(color_back, gray);
          copyColor(color_front, gray);
        }

          /* assign vertex coordinates */
        assignVertex( (RubiksCube[i][j][k]).vertex[0], x, y, z);
        assignVertex( (RubiksCube[i][j][k]).vertex[1], x+CUBESIZE, y, z);
        assignVertex( (RubiksCube[i][j][k]).vertex[2], x+CUBESIZE, y+CUBESIZE, z);
        assignVertex( (RubiksCube[i][j][k]).vertex[3], x, y+CUBESIZE, z);
        assignVertex( (RubiksCube[i][j][k]).vertex[4], x, y, z+CUBESIZE);
        assignVertex( (RubiksCube[i][j][k]).vertex[5], x+CUBESIZE, y, z+CUBESIZE);
        assignVertex( (RubiksCube[i][j][k]).vertex[6], x+CUBESIZE, y+CUBESIZE, z+CUBESIZE);
        assignVertex( (RubiksCube[i][j][k]).vertex[7], x, y+CUBESIZE, z+CUBESIZE);

          /* assign surface color */
        copyColor( (RubiksCube[i][j][k]).facecolor[BACK], color_back);
        copyColor( (RubiksCube[i][j][k]).facecolor[FRONT], color_front);
        copyColor( (RubiksCube[i][j][k]).facecolor[RIGHT], color_right);
        copyColor( (RubiksCube[i][j][k]).facecolor[LEFT], color_left);
        copyColor( (RubiksCube[i][j][k]).facecolor[BOTTOM], color_bottom);
        copyColor( (RubiksCube[i][j][k]).facecolor[TOP], color_top);

      }  /* Z axis */
    }  /* y axis */
  }  /* x axis */
}  /* end of initCube */


/*************************************************************************
  print out the vertex coordinates and face color for the cube model
**************************************************************************/
void printModel(void)
{
  int i, j, k;
  for (i=0; i<CUBEDIM; ++i) {
    for (j=0; j<CUBEDIM; ++j) {
      for (k=0; k<CUBEDIM; ++k) {
        printf("*****cube(%d,%d,%d)*****\n", i, j, k);
	printf("v0(%5.2f,%5.2f,%5.2f), v1(%5.2f,%5.2f,%5.2f),\n\
v2(%5.2f,%5.2f,%5.2f), v3(%5.2f,%5.2f,%5.2f)\n", \
		RubiksCube[i][j][k].vertex[0][0], \
		RubiksCube[i][j][k].vertex[0][1], \
		RubiksCube[i][j][k].vertex[0][2], \
		RubiksCube[i][j][k].vertex[1][0], \
                RubiksCube[i][j][k].vertex[1][1], \
                RubiksCube[i][j][k].vertex[1][2], \
		RubiksCube[i][j][k].vertex[2][0], \
                RubiksCube[i][j][k].vertex[2][1], \
                RubiksCube[i][j][k].vertex[2][2], \
		RubiksCube[i][j][k].vertex[3][0], \
                RubiksCube[i][j][k].vertex[3][1], \
                RubiksCube[i][j][k].vertex[3][2]);
	printf("v4(%5.2f,%5.2f,%5.2f), v5(%5.2f,%5.2f,%5.2f),\n\
v6(%5.2f,%5.2f,%5.2f), v7(%5.2f,%5.2f,%5.2f)\n",
                RubiksCube[i][j][k].vertex[4][0], \
                RubiksCube[i][j][k].vertex[4][1], \
                RubiksCube[i][j][k].vertex[4][2], \
                RubiksCube[i][j][k].vertex[5][0], \
                RubiksCube[i][j][k].vertex[5][1], \
                RubiksCube[i][j][k].vertex[5][2], \
                RubiksCube[i][j][k].vertex[6][0], \
                RubiksCube[i][j][k].vertex[6][1], \
                RubiksCube[i][j][k].vertex[6][2], \
                RubiksCube[i][j][k].vertex[7][0], \
                RubiksCube[i][j][k].vertex[7][1], \
                RubiksCube[i][j][k].vertex[7][2]);
        printf("back(%5.2f,%5.2f,%5.2f), front(%5.2f,%5.2f,%5.2f),\
right(%5.2f,%5.2f,%5.2f),\nleft(%5.2f,%5.2f,%5.2f), ",
		RubiksCube[i][j][k].facecolor[BACK][0], \
		RubiksCube[i][j][k].facecolor[BACK][1], \
		RubiksCube[i][j][k].facecolor[BACK][2], \
                RubiksCube[i][j][k].facecolor[FRONT][0],\
                RubiksCube[i][j][k].facecolor[FRONT][1],\
                RubiksCube[i][j][k].facecolor[FRONT][2],\
                RubiksCube[i][j][k].facecolor[RIGHT][0],\
                RubiksCube[i][j][k].facecolor[RIGHT][1],\
                RubiksCube[i][j][k].facecolor[RIGHT][2],\
                RubiksCube[i][j][k].facecolor[LEFT][0],\
                RubiksCube[i][j][k].facecolor[LEFT][1],\
                RubiksCube[i][j][k].facecolor[LEFT][2]);
	printf("bottom(%5.2f,%5.2f,%5.2f), top(%5.2f,%5.2f,%5.2f)\n",\
		RubiksCube[i][j][k].facecolor[BOTTOM][0],\
		RubiksCube[i][j][k].facecolor[BOTTOM][1],\
                RubiksCube[i][j][k].facecolor[BOTTOM][2],\
                RubiksCube[i][j][k].facecolor[TOP][0],\
                RubiksCube[i][j][k].facecolor[TOP][1],\
                RubiksCube[i][j][k].facecolor[TOP][2]);
      }
    }
  }
}


/**********************************************************************
   Draw the 3-D view of one cube
**********************************************************************/
void drawOneCube(GLint i, GLint j, GLint k, GLenum mode) {

  glBegin(GL_QUADS);

    /* back side */
  glColor3fv(RubiksCube[i][j][k].facecolor[BACK]);
  glVertex3fv(RubiksCube[i][j][k].vertex[0]);
  glVertex3fv(RubiksCube[i][j][k].vertex[3]);
  glVertex3fv(RubiksCube[i][j][k].vertex[2]);
  glVertex3fv(RubiksCube[i][j][k].vertex[1]);

    /* front side */
  glColor3fv(RubiksCube[i][j][k].facecolor[FRONT]);
  glVertex3fv(RubiksCube[i][j][k].vertex[4]);
  glVertex3fv(RubiksCube[i][j][k].vertex[5]);
  glVertex3fv(RubiksCube[i][j][k].vertex[6]);
  glVertex3fv(RubiksCube[i][j][k].vertex[7]);

    /* right side */
  glColor3fv(RubiksCube[i][j][k].facecolor[RIGHT]);
  glVertex3fv(RubiksCube[i][j][k].vertex[1]);
  glVertex3fv(RubiksCube[i][j][k].vertex[2]);
  glVertex3fv(RubiksCube[i][j][k].vertex[6]);
  glVertex3fv(RubiksCube[i][j][k].vertex[5]);

    /* left side */
  glColor3fv(RubiksCube[i][j][k].facecolor[LEFT]);
  glVertex3fv(RubiksCube[i][j][k].vertex[0]);
  glVertex3fv(RubiksCube[i][j][k].vertex[4]);
  glVertex3fv(RubiksCube[i][j][k].vertex[7]);
  glVertex3fv(RubiksCube[i][j][k].vertex[3]);

    /* bottom side */
  glColor3fv(RubiksCube[i][j][k].facecolor[BOTTOM]);
  glVertex3fv(RubiksCube[i][j][k].vertex[0]);
  glVertex3fv(RubiksCube[i][j][k].vertex[1]);
  glVertex3fv(RubiksCube[i][j][k].vertex[5]);
  glVertex3fv(RubiksCube[i][j][k].vertex[4]);

    /* top side */
  glColor3fv(RubiksCube[i][j][k].facecolor[TOP]);
  glVertex3fv(RubiksCube[i][j][k].vertex[3]);
  glVertex3fv(RubiksCube[i][j][k].vertex[7]);
  glVertex3fv(RubiksCube[i][j][k].vertex[6]);
  glVertex3fv(RubiksCube[i][j][k].vertex[2]);

  glEnd();

}

/**********************************************************************
   Draw the Rubik's cube
**********************************************************************/
void drawRubiksCube(GLenum mode) {
  GLint i, j, k;

  for (i=0; i<CUBEDIM; ++i) {
    for (j=0; j<CUBEDIM; ++j) {
      for (k=0; k<CUBEDIM; ++k) {
        drawOneCube(i, j, k, mode);
      }
    }
  }
}

/**********************************************************************
   Draw one slice of the Rubik's cube
**********************************************************************/
void drawOneSlice(char rotateAxis, GLint sliceNum, GLenum mode)
{
  GLint i, j, k;

  switch (rotateAxis) {
    case 'x': ;
    case 'X': i = sliceNum;
              for (j=0; j<CUBEDIM; ++j) {
                for (k=0; k<CUBEDIM; ++k)
                  drawOneCube(i, j, k, mode);
              }
              break;
    case 'y': ;
    case 'Y': j = sliceNum;
              for (i=0; i<CUBEDIM; ++i) {
                for (k=0; k<CUBEDIM; ++k)
                  drawOneCube(i, j, k, mode);
              }
              break;
    case 'z': ;
    case 'Z': k = sliceNum;
              for (i=0; i<CUBEDIM; ++i) {
                for (j=0; j<CUBEDIM; ++j)
                  drawOneCube(i, j, k, mode);
              }
  }
}


/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define MAIN_MENU_QUIT 0
#define WINDOW_X_SIZE 400
#define WINDOW_Y_SIZE 400
#define BUFSIZE 512

void initCube(void);
void drawRubiksCube(GLenum mode);
void drawOneSlice(char rotateAxis, GLint sliceNum, GLenum mode);
void printModel(void);
void get_face_and_ijk_of_cube(GLfloat x, GLfloat y, GLfloat z,
 			      GLint *face, GLint *i, GLint *j, GLint *k);
void get_slice_spin_direction(GLfloat clickx, GLfloat clicky, GLfloat clickz,
                              GLfloat x, GLfloat y, GLfloat z, GLint face,
                              GLint *dir);
void get_slice_spin_angle(GLint face, GLint spin_dir, GLfloat *spin);
void get_slice_spin_axis(GLint face, GLint spin_dir, char *axis);
void draw_slices(GLfloat spin, char axis, GLint i, GLint j, GLint k);
void update_slice(GLint face, char axis, GLint i, GLint j, GLint k,
                  GLfloat spin);
void scramble_rubik_cube(void);

GLfloat SliceSpin = 0.0;
GLfloat CubeSpinX = 0.0,  CubeSpinY = 0.0;

GLint MouseX, MouseY, PrevX, PrevY;
char RotationAxis;
GLint CubeI, CubeJ, CubeK;
/* back = 0, front = 1, right = 2, left = 3, bottom = 4, top = 5 */
GLint CubeFace;
/* up = 0, down = 1, left = 2, right = 3 */
GLint SliceSpinDirection = -1;
GLint DemoMode = 0;
GLint RotateCubeMode = 1;
GLint RotateSliceMode = 0;

/***************************************************************/
/* Initialize stuff											   */
/***************************************************************/

void init(void)
{
  glClearColor (0.0, 0.0, 0.0, 0.0);

  /* enable z-buffering */
  glEnable(GL_DEPTH_TEST);

  /* flat shading */
  glShadeModel (GL_FLAT);
}

/***************************************************************/
/* reshape function											   */
/***************************************************************/

void myReshape(GLsizei w, GLsizei h)
{
  GLfloat aspect;

  /* set the viewport to the size of the X-window */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /* set the window coordinates */
  aspect = (GLfloat)w / (GLfloat)h;
  /*    gluPerspective(60.0, aspect, 0.5, 50.0);*/
  glOrtho(-5.0, 5.0, -5.0, 5.0, -15.0, 15.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadIdentity();
}

/***************************************************************/
/* This function is called by "glut" whenever a key is pressed */
/***************************************************************/

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 27:					/* esc key */
    exit(0);
    break;
  }
}

/***************************************************************/
/* This function is called by "glut" whenever a key is pressed */
/***************************************************************/

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPushMatrix();

  if (DemoMode) {
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
/*     glRotatef(DemoSpin, 0.5, 0.5, 0.5); */
    glRotatef(CubeSpinX, 1.0, 0.0, 0.0);
    glRotatef(CubeSpinY, 0.0, 1.0, 0.0);
    drawRubiksCube(GL_RENDER);
  }
  else if (RotateCubeMode) {
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(CubeSpinX, 1.0, 0.0, 0.0);
    glRotatef(CubeSpinY, 0.0, 1.0, 0.0);
    drawRubiksCube(GL_RENDER);
  }
  else if (RotateSliceMode) {
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(CubeSpinX, 1.0, 0.0, 0.0);
    glRotatef(CubeSpinY, 0.0, 1.0, 0.0);
    get_slice_spin_axis(CubeFace, SliceSpinDirection, &RotationAxis);
    draw_slices(SliceSpin, RotationAxis, CubeI, CubeJ, CubeK);
  }
  glutSwapBuffers();
}

/***************************************************************/
/* callback for mouse click.								   */
/***************************************************************/

void mouse(int button, int state, int x, int y) {
  GLint viewport[4];
  GLdouble model[16], proj[16];
  GLfloat z;
  GLdouble wx, wy, wz;

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);

    MouseX = x;
    MouseY = viewport[3]-y;
    PrevX = x;
    PrevY = y;

    glReadPixels(x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    gluUnProject((GLfloat)x, (GLfloat) viewport[3]-y, z,
		     model, proj, viewport, &wx, &wy, &wz);

    get_face_and_ijk_of_cube(wx, wy, wz, &CubeFace, &CubeI, &CubeJ, &CubeK);

    if (CubeI == -1 && CubeJ == -1 && CubeK == -1) {
      RotateCubeMode = 1;
      RotateSliceMode = 0;
    }
    else {
      RotateCubeMode = 0;
      RotateSliceMode = 1;
    }
    fflush(stdout);
  }
  else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    if (RotateSliceMode)
      update_slice(CubeFace, RotationAxis, CubeI, CubeJ, CubeK, SliceSpin);
    RotateCubeMode = 1;
    RotateSliceMode = 0;
    SliceSpinDirection = -1;
    SliceSpin = 0.0;
    glutPostRedisplay();
  }
}

/***************************************************************/
/* callback for passive mouse motion.						   */
/***************************************************************/

void motion(int x, int y) {
  GLint viewport[4];
  GLdouble model[16], proj[16];
  int deltax;
  int deltay;
  GLfloat z;
  GLdouble wx, wy, wz;
  static int count = 0;
  GLdouble ClickWx, ClickWy, ClickWz;

  if (RotateCubeMode) {
    deltax = x - MouseX;
    deltay = y - MouseY;
    MouseX = x;
    MouseY = y;
    if (abs(deltax) > abs(deltay)) {	/* rotate about y-axis */
      if (deltax > 0)
        CubeSpinY += 3.0;
      else
        CubeSpinY -= 3.0;
      if (CubeSpinY > 360.0)
        CubeSpinY -= 360.0;
      if (CubeSpinY < 0.0)
        CubeSpinY += 360.0;
    }
    else {				/* rotate about x-axis */
      if (deltay < 0)
        CubeSpinX += 1.0;
      else
        CubeSpinX -= 1.0;
      if (CubeSpinX > 360.0)
        CubeSpinX -= 360.0;
      if (CubeSpinX < 0.0)
        CubeSpinX += 360.0;
    }
  }
  else if (RotateSliceMode) {
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);

    glReadPixels(PrevX, viewport[3]-PrevY, 1, 1, GL_DEPTH_COMPONENT,
		   GL_FLOAT, &z);
    gluUnProject((GLfloat)PrevX, (GLfloat) viewport[3]-PrevY, z, model,
		   proj, viewport, &ClickWx, &ClickWy, &ClickWz);
    glReadPixels(x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    gluUnProject((GLfloat)x, (GLfloat) viewport[3]-y, z, model, proj,
			viewport, &wx, &wy, &wz);

    get_slice_spin_direction(ClickWx, ClickWy, ClickWz, wx, wy, wz, CubeFace,
				 &SliceSpinDirection);
    get_slice_spin_angle(CubeFace, SliceSpinDirection, &SliceSpin);
    if (SliceSpin > 360.0)
      SliceSpin -= 360.0;
    if (SliceSpin < 0.0)
      SliceSpin += 360.0;
    PrevX = x;
    PrevY = y;
  }
  glutPostRedisplay();
}

/***************************************************************/
/* idle action:  spin the cube while in demo mode.			   */
/***************************************************************/

void spinDisplay(void)
{
  if (DemoMode) {
    CubeSpinX = CubeSpinX + 1.0;
    CubeSpinY = CubeSpinY + 1.0;
    if (CubeSpinX > 360.0)
      CubeSpinX = CubeSpinX - 360.0;
    if (CubeSpinY > 360.0)
      CubeSpinY = CubeSpinY - 360.0;
    glutPostRedisplay();
  }
}

/***************************************************************/
/* process the main menu.									   */
/***************************************************************/

static void selectMainMenu(int selection)
{
  switch (selection) {
  case MAIN_MENU_QUIT:
    exit(0);
  case 1:
    scramble_rubik_cube();
    break;
  case 2: 			/* rotate cube */
    DemoMode = 1;
    RotateCubeMode = 0;
    RotateSliceMode = 0;
    break;
  case 3:			/* stop rotate */
    DemoMode = 0;
    RotateCubeMode = 1;
    RotateSliceMode = 0;
    break;
  case 4: 			/* reset */
    DemoMode = 0;
    RotateCubeMode = 1;
    RotateSliceMode = 0;
    CubeSpinX = CubeSpinY = 0.0;
    initCube();
    break;
  }
  glutPostRedisplay();
}

/***************************************************************/
/* MAIN FUNCTION											   */
/***************************************************************/

int main(int argc, char** argv)
{

    /* use glut to create a 400x400 window with single buffering, rgb color */
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (WINDOW_X_SIZE, WINDOW_Y_SIZE);
    glutInitWindowPosition (100, 100);
    glutCreateWindow(argv[0]);
    init();

    initCube();

    /* set callback to draw OpenGL */
    glutDisplayFunc(display);

    /* set callback when window resized */
    glutReshapeFunc(myReshape);

    /* set callback for keyboard input */
    glutKeyboardFunc(keyboard);

    /* animation callback */
    glutIdleFunc(spinDisplay);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    /* create the main menu */
    glutCreateMenu(selectMainMenu);

    glutAddMenuEntry("Scramble", 1);
    glutAddMenuEntry("Rotate Cube", 2);
    glutAddMenuEntry("Stop Rotate", 3);
    glutAddMenuEntry("Reset", 4);

    /* add quit option to main menu */
    glutAddMenuEntry("Quit", MAIN_MENU_QUIT);

    /* when right mouse button is pressed, menu will appear */
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /* start glut's event loop */
    glutMainLoop();

    return(0);
}
