// Alex Tzinov
#include "final.h"
#include "CSCIx229.h"


int drawSkel = 0;
unsigned int texture[7]; // Texture names

#define Dfloor  16
#define YfloorMin -8
#define YfloorMax 12


void Print(const char* format , ...)
{
  char    buf[LEN];
  char*   ch=buf;
  va_list args;
  //  Turn the parameters into a character string
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);
  //  Display the characters one at a time at the current raster position
  while (*ch)
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*[0]
*  Set projection
*/
static void Project()
{
  //  Tell OpenGL we want to manipulate the projection matrix
  //  Projection matrix is used to manipulate the matrix that deals with the view volume / camera properties
  //  Typically set zoom factor, aspect ratio, and clipping planes
  //  Sets up what we want t see from the scene that we create. Objects can exist anywhere in the world but will only be
  //  visible inside this view volume
  //  We are essentially creating 6 clipping planes that define what we see.
  //  Left wall, right wall, floor, ceiling, back wall, front wall
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Perspective transformation
  if (mode)
  gluPerspective(fov,asp,dim/8,8*dim);
  //  Orthogonal projection
  else
  glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
  //  Switch to manipulating the model matrix
  //  The model matrix is used to make transformations to the models/objects in the world.
  //  We can define object once, and then rotate, scale, transform it.
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}

void cubeSide(double squareSize)
{
  // TODO: Have some notion for randomly choosing colors for each square

  double squareDisplacement = (squareSize*2) + spacing;
  // This displacement is such because:
  // Moving each subsequent square over by a value of 1 squareSize will result in the square perfectly overlapping HALF of the other square (since squareSize represents HALF of the full square size)
  // Moving each subsequent square over by a value of 2 squareSizes will result in the square perfectly aligning itself RIGHT NEXT to the other square
  // Adding then the spacing value will give enough spacing between each square to draw the black frame


  // top row
  square(squareDisplacement,squareDisplacement,0 ,squareSize,squareSize,squareSize, 0);
  square(0,squareDisplacement,0 ,squareSize,squareSize,squareSize, 0);
  square(-squareDisplacement,squareDisplacement,0 ,squareSize,squareSize,squareSize, 0);
  // bottom row
  square(squareDisplacement,-squareDisplacement,0 ,squareSize,squareSize,squareSize, 0);
  square(0,-squareDisplacement,0 ,squareSize,squareSize,squareSize, 0);
  square(-squareDisplacement,-squareDisplacement,0 ,squareSize,squareSize,squareSize, 0);

  // middle row
  square(-squareDisplacement,0,0 ,squareSize,squareSize,squareSize, 0);
  square(0,0,0 ,squareSize,squareSize,squareSize, 0);
  square(squareDisplacement,0,0 ,squareSize,squareSize,squareSize, 0);

  if (drawSkel == 0) {
    glColor3f(0,0,0);
    square((squareSize + (spacing/2)),0, 0 , (spacing/2), ((spacing) +(squareSize*3)),squareSize, 0);
    square(-(squareSize + (spacing/2)),0, 0 , (spacing/2), ((spacing) +(squareSize*3)),squareSize, 0);

    square(0,(squareSize+(spacing/2)), 0 , ((spacing) +(squareSize*3)), (spacing/2),squareSize, 0);
    square(0,-(squareSize+(spacing/2)), 0 , ((spacing) +(squareSize*3)), (spacing/2),squareSize, 0);
  }

}

/*
*  Draw vertex in polar coordinates with normal
*/
/* What this function will be useful for is placing a vertex (and the corresponding normal for that vertex)
* anywhere in the scene. Because it is using polar coordinates, we can easily construct an entire sphere made up of vertexes and their corresponding normals.
* Once our entire sphere is made up of normals, we can send light at it and have behave as a 3D sphere would (because it has all (or several thousands) of its normal surfaces defined).
*/
static void Vertex(double th,double ph)
{
  double x = Sin(th)*Cos(ph);
  double y = Cos(th)*Cos(ph);
  double z =         Sin(ph);
  //  For a sphere at the origin, the position
  //  and normal vectors are the same
  glNormal3d(x,y,z);
  glVertex3d(x,y,z);
}


static void sun(double x,double y,double z,double r)
{
  int th,ph;
  float yellow[] = {1.0,1.0,0.0,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(r,r,r);
  //  Yellow sun
  glColor3f(1,1,.6);
  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
  //  Bands of latitude
  //  Generate bands of rectangles, each rectangle getting a respective surface normal set
  //  If we decrease the granularity of these rectangles (by increasing the value of inc) and turn off shading (set smooth to 0) we can very clearly see each one of these quad bands and how the color of the quad band is determined by the angle of the light. This angle is relative to the surface normal of the quad which gets set here
  for (ph=-90;ph<90;ph+=inc)
  {
    glBegin(GL_QUAD_STRIP);
    for (th=0;th<=360;th+=2*inc)
    {
      Vertex(th,ph);
      Vertex(th,ph+inc);
    }
    glEnd();
  }
  //  Undo transofrmations
  glPopMatrix();
}


void drawDesk() {
  // Draw the four legs
  drawTableLeg(-4,-5,2,1,1,1,0);
  drawTableLeg(4,-5,2,1,1,1,0);
  drawTableLeg(-4,-5,-2,1,1,1,0);
  drawTableLeg(4,-5,-2,1,1,1,0);
  drawTableTop(0, .3, .7, 1, 1, .5, 0);
  drawTableTop(0, 0, .7, 1, 1, .5, 0);
  drawTableSides(0, -2, -1.80, 1, 1, 1, 0);
  drawTableSides(0, -2, 3.20, 1, 1, 1, 0);
  drawTableSides(-5, -2, .70, .5, 1, 1, 90);
  drawTableSides(5, -2, .70, .5, 1, 1, 90);
}

void drawTableLeg(double x, double y, double z, double dx, double dy, double dz, double th) {
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);

  drawTableLegSide(0, 0, 0, .3, 3, 1, 0);
  drawTableLegSide(-.7, 0, .7, .3, 3, 1, 90);
  drawTableLegSide(.7, 0, .7, .3, 3, 1, -90);
  drawTableLegSide(0, 0, 1.4, .3, 3, 1, 180);
  glPopMatrix();
}
// The table top will be made up two large horizontal rectangular planes that extend BEYOND
// the table legs, and have four smaller rectangles at each side. (A 3D rectangular slab)


void drawTableTop(double x, double y, double z, double dx, double dy, double dz, double th) {
  int i,j;
  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);
  int tableSize = 5;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[3]);
  glPolygonOffset(1,1);
  glColor3f(1,1,1);
  glNormal3f(1,0,0);
  for (j=-tableSize;j<tableSize;j++)
  {
    glBegin(GL_QUAD_STRIP);
    for (i=-tableSize;i<=tableSize;i++)
    {
      glTexCoord2f(i,j); glVertex3f(i,-2,j);
      glTexCoord2f(i,j+1); glVertex3f(i,-2,j+1);
    }
    glEnd();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}




void drawTableSides(double x, double y, double z, double dx, double dy, double dz, double th) {
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[3]);
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);


  glBegin(GL_QUADS);

  glTexCoord2f(0,0);
  glVertex3f(-5, 0, 0);

  glTexCoord2f(1,0);
  glVertex3f(-5, .30, 0);

  glTexCoord2f(1,1);
  glVertex3f(5, .30, 0);

  glTexCoord2f(0,1);
  glVertex3f(5, 0, 0);
  glNormal3d(1,0,0);
  glEnd();
  //  Undo transofrmations
  glPopMatrix();
}

// Draws an individual table leg which consists of four vertical rectangles with a table leg at the bottom of each one
// The table leg can be either a fatter cube towards the bottom, or a square, flat topped pyarmid (four trapezoids)
void drawTableLegSide(double x, double y, double z, double dx, double dy, double dz, double th) {

  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);


  glBegin(GL_QUADS);

  glTexCoord2f(0,0);
  glVertex3f(-1,-1, 1);

  glTexCoord2f(1,0);
  glVertex3f(+1,-1, 1);

  glTexCoord2f(1,1);
  glVertex3f(+1,+1, 1);

  glTexCoord2f(0,1);
  glVertex3f(-1,+1, 1);
  glNormal3d(0,0,-1);
  glEnd();
  //  Undo transofrmations
  glPopMatrix();


}


void drawSides(double squareSize, int bw, double x, double y, double z, double dx, double dy, double dz, double th, double ph) {


  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(ph,0,0,1);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  double fullSquareSize = squareSize * 2;
  double squareDisplacement = fullSquareSize + spacing;

  // ORANGE - FRONT
  (bw == 0) ? glColor3f(.2,.2,.2) : glColor3f(1,.6,0);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  cubeSide(squareSize);


  glBindTexture(GL_TEXTURE_2D,texture[5]);
  glRotatef(90,0,1,0);
  glTranslated(squareDisplacement,0,squareDisplacement);
  // YELLOW - RIGHT
  (bw == 0) ? glColor3f(.35,.35,.35) : glColor3f(1,1,0);
  cubeSide(squareSize);
  glDisable(GL_TEXTURE_2D);


  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  glRotatef(-180,0,1,0);
  glTranslated(0,0,((fullSquareSize*2)+(spacing*2)));
  // WHITE - LEFT
  glColor3f(1,1,1);
  cubeSide(squareSize);

  glBindTexture(GL_TEXTURE_2D,texture[4]);
  glRotatef(-90,0,1,0);
  glTranslated(-squareDisplacement,0,squareDisplacement);
  // RED - BACK
  (bw == 0) ? glColor3f(.55,.55,.55) : glColor3f(1,0,0.2);
  cubeSide(squareSize);


  glBindTexture(GL_TEXTURE_2D,texture[1]);
  glRotatef(90,1,0,0);
  glTranslated(0,-squareDisplacement,-((fullSquareSize*2)+spacing));
  // GREEN - TOP
  (bw == 0) ? glColor3f(.1,.1,.1) : glColor3f(.5,.8,.2);
  cubeSide(squareSize);


  glBindTexture(GL_TEXTURE_2D,texture[3]);
  glNormal3d(0,0,1);
  glTranslated(0,0,((fullSquareSize*3)+(spacing*2)));
  // BLUE - BOTTOM
  (bw == 0) ? glColor3f(.8,.8,.8) : glColor3f(0,0,.8);
  cubeSide(squareSize);

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}


static void square(double x,double y,double z, double dx,double dy,double dz, double th) {
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  //glRotated(th,0,1,0);
  glScaled(dx,dy,dz);


  glBegin(GL_QUADS);

  glTexCoord2f(0,0);
  glVertex3f(-1,-1, 1);

  glTexCoord2f(1,0);
  glVertex3f(+1,-1, 1);

  glTexCoord2f(1,1);
  glVertex3f(+1,+1, 1);

  glTexCoord2f(0,1);
  glVertex3f(-1,+1, 1);
  glEnd();
  //  Undo transofrmations
  glPopMatrix();
}

/*
*  OpenGL (GLUT) calls this routine to display the scene
*/
void display()
{
  const double len=1.5;  //  Length of axes
  //  Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glClearColor( .2, .2, .2, 1);
  //  Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  //  Undo previous transformations
  glLoadIdentity();
  //  Perspective - set eye position
  if (mode)
  {
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    glTranslated(-xpos, -ypos, -zpos);
  }
  //  Orthogonal - set world orientation
  else
  {
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
    glTranslated(-xpos, -ypos, -zpos);
  }
  //  Draw A cube

  //  Flat or smooth shading
  glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

  if (light) {
    float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,.5};
    float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
    float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
    float Position[]  = {sunDistance*Cos(zh),sunElevation,sunDistance*Sin(zh),1.0};
    //  Draw light position as ball (still no lighting here)

    /* The light obviously doesn't come from physically creating the sun object below in the scene. Whether we draw this sun object or not is not significant. However, the scene does make more sense if the user can see where the light is actively coming from. We acheive this illusion (openGL lighting seemingly 'coming' from our sun) by using the idle function to update the physical position of the sun, and then simultanously updating the lighting position as well*/
    if (seeSun)
    sun(Position[0],Position[1],Position[2] , 0.3);

    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
    glEnable(GL_LIGHT0);
    //  Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,Position);

  }
  else {
    glDisable(GL_LIGHTING);
  }

  drawFloor();
  drawCeiling();
  drawWalls(0,0,0,1,1,1,0, 0);
  drawWalls(0,0,0,1,1,1,90, 0);
  drawWalls(0,0,0,1,1,1,-90, 0);
  drawWalls(0,0,0,1,1,1,180, 1);



  drawDesk();
  glRotated(zh, 0, .1, 0);
  glTranslated(1.65, 0, -1.6);
  drawSides(.3, 1, -1, .6, 2, 1, 1, 1, 45, 45);
  //glTranslated(0,4,-4);
  //  Draw axes
  glColor3f(1,1,1);
  if (axes)
  {
    glBegin(GL_LINES);
    glVertex3d(0.0,0.0,0.0);
    glVertex3d(len,0.0,0.0);
    glVertex3d(0.0,0.0,0.0);
    glVertex3d(0.0,len,0.0);
    glVertex3d(0.0,0.0,0.0);
    glVertex3d(0.0,0.0,len);
    glEnd();
    //  Label axes
    glRasterPos3d(len,0.0,0.0);
    Print("X");
    glRasterPos3d(0.0,len,0.0);
    Print("Y");
    glRasterPos3d(0.0,0.0,len);
    Print("Z");
  }
  //  Display parameters

  glColor3f(.2,.3,.8);
  glWindowPos2i(5,5);
  Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");;
  //  Render the scene and make it visible

  glFlush();
  glutSwapBuffers();

}


void drawFloor() {
  //  Draw floor
  int i,j;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[5]);
  glPolygonOffset(1,1);
  glColor3f(1,1,1);
  glNormal3f(0,1,0);
  for (j=-Dfloor;j<Dfloor;j++)
  {
    glBegin(GL_QUAD_STRIP);
    for (i=-Dfloor;i<=Dfloor;i++)
    {
      glVertex3f(i,YfloorMin,j);
      glVertex3f(i,YfloorMin,j+1);
      //glTexCoord2f(i,j+8); glVertex3f(i,YfloorMin,j+8);
    }
    glEnd();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_TEXTURE_2D);
}

void drawWalls(double x,double y,double z, double dx,double dy,double dz, double th, int style) {
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  if (style == 1) glBindTexture(GL_TEXTURE_2D,texture[6]);
  else glBindTexture(GL_TEXTURE_2D,texture[2]);
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  //glScaled(dx,dy,dz);


  glBegin(GL_QUADS);

  glTexCoord2f(0,0);
  glVertex3f(-Dfloor, YfloorMin, Dfloor);

  glTexCoord2f(1,0);
  glVertex3f(-Dfloor, YfloorMax, Dfloor);

  glTexCoord2f(1,1);
  glVertex3f(Dfloor, YfloorMax, Dfloor);

  glTexCoord2f(0,1);
  glVertex3f(Dfloor, YfloorMin, Dfloor);
  glNormal3d(1,0,0);
  glEnd();
  //  Undo transofrmations
  glPopMatrix();
}

void drawCeiling() {
  // draw ceiling
  int i,j;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[4]);
  glPolygonOffset(1,1);
  glColor3f(1,1,1);
  glNormal3f(1,0,0);
  for (j=-Dfloor;j<Dfloor;j++)
  {
    glBegin(GL_QUAD_STRIP);
    for (i=-Dfloor;i<=Dfloor;i++)
    {
      glTexCoord2f(i,j); glVertex3f(i,YfloorMax,j);
      glTexCoord2f(i,j+1); glVertex3f(i,YfloorMax,j+1);
    }
    glEnd();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_TEXTURE_2D);
}

/*
*  GLUT calls this routine when an arrow key is pressed
*/
void special(int key,int x,int y)
{
  //  Right arrow key - increase angle by 5 degrees
  if (key == GLUT_KEY_RIGHT)
  th += 5;
  //  Left arrow key - decrease angle by 5 degrees
  else if (key == GLUT_KEY_LEFT) {
    th -= 5;
  }
  //  Up arrow key - increase elevation by 5 degrees
  else if (key == GLUT_KEY_UP) {
    if (ph < 45) ph += 5;
  }
  //  Down arrow key - decrease elevation by 5 degrees
  else if (key == GLUT_KEY_DOWN) {
    if (ph > -25) ph -= 5;
}
  //  PageUp key - increase dim
  else if (key == GLUT_KEY_PAGE_UP) {
    if (dim < 8.4) dim += 0.1;
}
  //  PageDown key - decrease dim
  else if (key == GLUT_KEY_PAGE_DOWN && dim>1) {
    if (dim > 4.4) dim -= 0.1;
}
  //  Keep angles to +/-360 degrees
  th %= 360;
  ph %= 360;
  //  Update projection
  Project();

  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
*  GLUT calls this routine when a key is pressed
*/
void key(unsigned char ch,int x,int y)
{

  glLoadIdentity();

  //  Reset everything to starting point
  if (ch == '0') {
    th = 30;
    ph = 30;
    dim = 6.0;
    xpos = 0;
    ypos = 0;
    zpos = 0;
    sunSpeed = 1000.0;
    sunDistance = 5;
    sunElevation = 1;
  }


  //  Exit on ESC
  else if (ch == 27)
  exit(0);

  /* The radius of the suns orbit */
  else if (ch == 'd')
  sunDistance += 1;
  else if (ch == 'D')
  sunDistance -= 1;

  /* Suns elevation (Y displacement) */
  else if (ch == 'e')
  sunElevation += 1;
  else if (ch == 'E')
  sunElevation -= 1;

  /* Enable Disable Sun */
  else if (ch == 'q')
  light = 1-light;

  /* Enable Disable Sun Movement */
  else if (ch == 'w')
  sunMovement = 1 - sunMovement;

  //  Toggle axes
  else if (ch == 'a' || ch == 'A')
  axes = 1-axes;

  //  Switch display mode
  else if (ch == 'm' || ch == 'M')
  mode = 1-mode;

  //  Change field of view angle
  else if (ch == '-' && ch>1)
  fov--;
  else if (ch == '+' && ch<179)
  fov++;

  // Manually move the sun
  else if (ch == '>' )
  zh--;
  else if (ch == '<' )
  zh++;

  // Toggle Sun Speed
  else if (ch == 'Z')
  sunSpeed += 100;
  else if (ch == 'z') {
    sunSpeed -= 100;
    if (sunSpeed == 0)
    sunSpeed = 100;
  }

  // Toggle Skeletons
  else if (ch == 's')
  drawSkel = 1-drawSkel;
  else if (ch == 'c')
  seeSun = 1 - seeSun;

  /* Strafing left and right */
  if (ch=='k') {
    float yrotrad;
    yrotrad = (th * (3.141592654f / 180));
    xpos += (float)(cos(yrotrad)) * 0.2;
    zpos += (float)(sin(yrotrad)) * 0.2;
  }

  if (ch=='j') {
    float yrotrad;
    yrotrad = (th * (3.141592654f / 180));
    xpos -= (float)(cos(yrotrad)) * 0.2;
    zpos -= (float)(sin(yrotrad)) * 0.2;
  }

  /* Navigating forward and backwards */
  else if (ch=='h')
  {
    float xrotrad, yrotrad;
    yrotrad = (th * (3.141592654f / 180));
    xrotrad = ((ph) * (3.141592654f / 180));
    /* Calculate Polar Coordinates */
    xpos -= (float)(cos(xrotrad)*(sin(yrotrad)));
    ypos += (float)(sin(xrotrad));
    zpos += (float)(cos(xrotrad)*cos(yrotrad));
  }

  else if (ch=='l')
  {
    float xrotrad, yrotrad;
    yrotrad = (th * (3.141592654f / 180));
    xrotrad = ((ph) * (3.141592654f / 180));
    /* Calculate Polar Coordinates */
    xpos += (float)(cos(xrotrad)*(sin(yrotrad)));
    ypos -= (float)(sin(xrotrad));
    zpos -= (float)(cos(xrotrad)*cos(yrotrad));
  }


  //  Reproject
  Project();
  glutIdleFunc(sunMovement?idle:NULL);
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
*  GLUT calls this routine when the window is resized
*/
void reshape(int width,int height)
{
  //  Ratio of the width to the height of the window
  asp = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Set projection
  Project();
}

/* This function gets called continously when the window isn't busy loading a window resize or some other action
* This idle() function is a good place to put continuous animiation that we want to have play out throughout the
* duration of the program */
void idle()
{
  //  Elapsed time in seconds
  double t = glutGet(GLUT_ELAPSED_TIME)/sunSpeed;
  zh = fmod(90*t,360.0);
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
*  Start up GLUT and tell it what to do
*/
int main(int argc,char* argv[])
{
  th=-255;
  ph=25;
  mode = 1;
  dim = 5.1;
  //  Initialize GLUT
  glutInit(&argc,argv);
  //  Request double buffered, true color window with Z buffering at 600x600
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(600,600);
  glutCreateWindow("Final Project Alex Tzinov");

  glClearColor( .5, .5, .5, 1);
  //  Set callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  //  Load textureG
  texture[0] = LoadTexBMP("dark_wood.bmp");
  texture[1] = LoadTexBMP("tile.bmp");
  texture[2] = LoadTexBMP("steel.bmp");
  texture[3] = LoadTexBMP("blackboard.bmp");
  texture[4] = LoadTexBMP("popcorn.bmp");
  texture[5] = LoadTexBMP("basketball-floor-texture.bmp");
  texture[6] = LoadTexBMP("chalkboard.bmp");
  //  texture[1] = LoadTexBMP("carbon.bmp");
  //  texture[2] = LoadTexBMP("carbon.bmp");
  //  texture[3] = LoadTexBMP("carbon.bmp");
  //  texture[4] = LoadTexBMP("carbon.bmp");
  //  texture[5] = LoadTexBMP("carbon.bmp");
  //  Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
