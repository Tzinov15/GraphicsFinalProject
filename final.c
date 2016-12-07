// Alex Tzinov
#include "final.h"
#include "CSCIx229.h"


int drawSkel = 0;
int drawFull = 0;

int newth = 0;
int oldth = 0;

int newph = 0;
int oldph = 0;

double newxpos = -1.545;
double oldxpos = -1.545;

double newypos = -.97;
double oldypos = -.97;

double newzpos = 11.573;
double oldzpos = 11.573;

double newdim = 12.5;
double olddim = 12.5;

unsigned int texture[10]; // Texture names

#define Dfloor  36
#define YfloorMin -12
#define YfloorMax 16


void Print(const char* format , ...) {
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
static void Project() {
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

/*
*  Draw vertex in polar coordinates with normal
What this function will be useful for is placing a vertex (and the corresponding normal for that vertex)
* anywhere in the scene. Because it is using polar coordinates, we can easily construct an entire sphere
made up of vertexes and their corresponding normals.
* Once our entire sphere is made up of normals, we can send light at it and have behave as a 3D sphere would
(because it has all (or several thousands) of its normal surfaces defined).
*/
static void Vertex(double th,double ph) {
  double x = Sin(th)*Cos(ph);
  double y = Cos(th)*Cos(ph);
  double z =         Sin(ph);
  //  For a sphere at the origin, the position
  //  and normal vectors are the same
  glNormal3d(x,y,z);
  glVertex3d(x,y,z);
}


static void cubeBall(double x,double y,double z,double r) {
  int th,ph;
  float yellow[] = {1.0,1.0,0.0,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(r,r,r);
  //  Yellow sun
  glColor3f(.2,.2,.2);
  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
  //  Bands of latitude
  //  Generate bands of rectangles, each rectangle getting a respective surface normal set
  //  If we decrease the granularity of these rectangles (by increasing the value of inc) and turn off shading
  //  (set smooth to 0) we can very clearly see each one of these quad bands and how the color of the quad band
  // is determined by the angle of the light. This angle is relative to the surface normal of the quad which gets
  //   set here
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

static void sun(double x,double y,double z,double r) {
  int th,ph;
  float yellow[] = {1.0,1.0,0.0,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(r,r,r);
  //  Yellow sun
  glColor3f(1,1,1);
  glMaterialf(GL_FRONT,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
  //  Bands of latitude
  //  Generate bands of rectangles, each rectangle getting a respective surface normal set
  //  If we decrease the granularity of these rectangles (by increasing the value of inc) and turn off shading
  //  (set smooth to 0) we can very clearly see each one of these quad bands and how the color of the quad band
  // is determined by the angle of the light. This angle is relative to the surface normal of the quad which gets
  //   set here
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

void cubeSide(double squareSize, double x, double y, double z, double th, double ph, double color[], int inOrOut) {
  // TODO: Have some notion for randomly choosing colors for each square

  double squareDisplacement = (squareSize*2) + spacing;
  /*
  This displacement is such because:
  Moving each subsequent square over by a value of 1 squareSize will result
  in the square perfectly overlapping HALF of the other square (since squareSize
  represents HALF of the full square size)

  Moving each subsequent square over by a value of 2 squareSizes will result in
  the square perfectly aligning itself RIGHT NEXT to the other square

  Adding then the spacing value will give enough spacing between each square to
   draw the black frame
  */

  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glRotated(ph,1,0,0);

  double dz = 1;

  glColor3f(color[0], color[1], color[2]);
  if (drawFull == 1) {
    square(squareDisplacement,squareDisplacement,0 ,squareSize,squareSize,dz, 0);
    square(0,squareDisplacement,0 ,squareSize,squareSize,dz, 0);
    square(-squareDisplacement,squareDisplacement,0 ,squareSize,squareSize,dz, 0);

    // bottom row
    square(squareDisplacement,-squareDisplacement,0 ,squareSize,squareSize,dz, 0);
    square(0,-squareDisplacement,0 ,squareSize,squareSize,dz, 0);
    square(-squareDisplacement,-squareDisplacement,0 ,squareSize,squareSize,dz, 0);

    // middle row
    square(-squareDisplacement,0,0 ,squareSize,squareSize,dz, 0);

  }
  square(0,0,0 ,squareSize,squareSize,dz, 0);
  if (inOrOut) square(0,0,.3 ,squareSize,squareSize,dz, 0);
  else  square(0,0,-.3 ,squareSize,squareSize,dz, 0);


  double insert;
  if (inOrOut) insert = .15;
  else insert = -.15;

  /* Bottom edge */
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  glColor3f(.2, .2, .2);
  glPushMatrix();

  glTranslated(0, -squareSize-.3, insert);
  glRotated(90, 1, 0, 0);
  glScaled(1, .1, 1);
  square(0,0,-.3 ,squareSize,squareSize,dz, 0);

  if (inOrOut) glTranslated(-squareSize+.3, 0, -squareSize-.3);
  else  glTranslated(-squareSize+.3, 0, -squareSize-.3);
  glRotated(90,0 , 1, 0);
  square(0,0,-.3 ,squareSize,squareSize,dz, 0);

  if (inOrOut) glTranslated(0, 0, squareSize*2);
  else glTranslated(0, 0, squareSize*2);
  square(0,0,-.3 ,squareSize,squareSize,dz, 0);

  glPopMatrix();

  glPushMatrix();



  glTranslated(0, squareSize+.3, insert);
  glRotated(-90, 1, 0, 0);
  glScaled(1, .1, 1);
  square(0,0,-.3 ,squareSize,squareSize,dz, 0);

  glPopMatrix();
  glPopAttrib();
  glColor3f(color[0], color[1], color[2]);



  if (drawFull == 1) {
    square(squareDisplacement,0,0 ,squareSize,squareSize,dz, 0);
  }

  if (drawSkel == 0) {
    glColor3f(0,0,0);
    square((squareSize + (spacing/2)),0, 0 , (spacing/2), ((spacing) +(squareSize*3)),squareSize, 0);
    square(-(squareSize + (spacing/2)),0, 0 , (spacing/2), ((spacing) +(squareSize*3)),squareSize, 0);

    square(0,(squareSize+(spacing/2)), 0 , ((spacing) +(squareSize*3)), (spacing/2),squareSize, 0);
    square(0,-(squareSize+(spacing/2)), 0 , ((spacing) +(squareSize*3)), (spacing/2),squareSize, 0);
  }

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
  glBindTexture(GL_TEXTURE_2D,texture[2]);


  drawCenterBall(squareSize);


  double cubeWidth = ((squareSize * 2) * 3) + (spacing * 2);

  double orange[3] = {1, .6, 0};
  double red[3] = {1, 0, .2}  ;
  double yellow[3] = {1, 1, 0};
  double white[3] = {1, 1, 1};
  double blue[3] = {.2, .2, 1};
  double green[3] = {.2, 1, .2};

  // ORANGE - FRONT
  (bw == 0) ? glColor3f(.2,.2,.2) : glColor3f(1,.6,0);
  glNormal3d(0, 0, 1);
  cubeSide(squareSize, x, y, z-(cubeWidth/2), 0, 0, orange, 1);

  // RED - BACK
  (bw == 0) ? glColor3f(.55,.55,.55) : glColor3f(1,0,0.2);
  glNormal3d(0, 0, 1);
  cubeSide(squareSize, x, y, (cubeWidth/2), 0, 0, red, 0);

  // YELLOW - RIGHT
  (bw == 0) ? glColor3f(.35,.35,.35) : glColor3f(1,1,0);
  glNormal3d(0, 0, 1);
  cubeSide(squareSize, (cubeWidth/2), y,z , 90, 0, yellow, 0);

  // WHITE - LEFT
  glColor3f(1,1,1);
  glNormal3d(0, 0, 1);
  cubeSide(squareSize, (-cubeWidth/2), y, z, 90, 0, white, 1);

  // GREEN - TOP
  (bw == 0) ? glColor3f(.1,.1,.1) : glColor3f(.5,.8,.2);
  glNormal3d(0, 0, 1);
  cubeSide(squareSize, x, cubeWidth/2, z, 0 ,90, green, 1);

  (bw == 0) ? glColor3f(.1,.1,.1) : glColor3f(.5,.8,.2);
  glNormal3d(0, 0, 1);
  cubeSide(squareSize, x, -cubeWidth/2, z, 0 ,90, blue, 0);

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}


void drawCenterBall(double squareSize) {
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); //Smooth polygons
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Best perspective corrections
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); //Smooth points
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); //Smooth lines
  GLUquadricObj *obj = gluNewQuadric();

  double cubeWidth = ((squareSize * 2) * 3) + (spacing * 2);

  double cylinderThickness = .5;
  double cylinderLength = cubeWidth -.1;
  glColor3f(.2, .2, .2);
  glPushMatrix();
  glTranslated(0, 0, -cylinderLength/2);
  gluQuadricNormals(obj, GLU_SMOOTH);
  gluCylinder(obj, cylinderThickness, cylinderThickness, cylinderLength, 24, 24);
  glTranslated(0, cylinderLength/2, cylinderLength/2);
  glRotated(90, 1, 0, 0);
  gluCylinder(obj, cylinderThickness, cylinderThickness, cylinderLength, 24, 24);
  glTranslated(-cylinderLength/2, 0, cylinderLength/2);
  glRotated(90, 0, 1, 0);
  gluCylinder(obj, cylinderThickness, cylinderThickness, cylinderLength, 24, 24);
  glPopMatrix();

  cubeBall(0, 0, 0, 1);

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
  glVertex3f(-1,-1, 0);

  glTexCoord2f(1,0);
  glVertex3f(+1,-1, 0);

  glTexCoord2f(1,1);
  glVertex3f(+1,+1, 0);

  glTexCoord2f(0,1);
  glVertex3f(-1,+1, 0);
  glEnd();
  //  Undo transofrmations
  glPopMatrix();
}

void drawDesk(double x,double y,double z, double dx,double dy,double dz, double th) {
  glPushMatrix();
  glTranslated(x,y,z);
  glScaled(dx,dy,dz);
  // Draw the four legs
  drawTableLeg((-4 + x),y,(2+z),1,1,1,0);
  drawTableLeg((4 + x),y,(2+z),1,1,1,0);
  drawTableLeg((-4 + x),y,(-2+z),1,1,1,0);
  drawTableLeg((4 + x),y,(-2+z),1,1,1,0);

  drawTableTop(x, 5.3+y, z+.7, 1, 1, .5, 0);
  drawTableTop(x, 5+y, z+.7, 1, 1, .5, 0);
  drawTableSides(x, 3+y, -1.80+z, 1, 1, 1, 0);
  drawTableSides(x, 3+y, 3.20+z, 1, 1, 1, 0);
  drawTableSides(-5+x, 3+y, .70+z, .5, 1, 1, 90);
  drawTableSides(x+5, 3+y, .70+z, .5, 1, 1, 90);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void drawTableLeg(double x, double y, double z, double dx, double dy, double dz, double th) {
  glPushMatrix();
  //  Offset
  glRotated(th,0,1,0);
  glTranslated(x,y,z);
  glScaled(dx,dy,dz);

  drawTableLegSide(0, 0, 0, .3, 3, 1, 0);
  drawTableLegSide(-.7, 0, .7, .3, 3, 1, 90);
  drawTableLegSide(.7, 0, .7, .3, 3, 1, -90);
  drawTableLegSide(0, 0, 1.4, .3, 3, 1, 180);
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
  glBindTexture(GL_TEXTURE_2D,texture[0]);
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
  glDisable(GL_TEXTURE_2D);
  //  Undo transofrmations
  glPopMatrix();
}

// The table top will be made up two large horizontal rectangular planes that extend BEYOND
// the table legs, and have four smaller rectangles at each side. (A 3D rectangular slab)
void drawTableTop(double x, double y, double z, double dx, double dy, double dz, double th) {
  glPushMatrix();
  glRotated(th,0,1,0);
  glTranslated(x,y,z);
  glScaled(dx,dy,dz);
  int tableSize = 5;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[3]);
  glPolygonOffset(1,1);
  //glColor3f(1,1,1);
  glNormal3f(1,0,0);




  glBegin(GL_QUADS);

  glTexCoord2f(0,0);
  glVertex3f(-tableSize, -2, -tableSize);

  glTexCoord2f(1,0);
  glVertex3f(tableSize, -2, -tableSize);

  glTexCoord2f(1,1);
  glVertex3f(tableSize, -2, tableSize);

  glTexCoord2f(0,1);
  glVertex3f(-tableSize, -2, tableSize);
  glNormal3d(0,1,0);
  glEnd();

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


/*
*  OpenGL (GLUT) calls this routine to display the scene
*/
void display() {
  const double len=1.5;  //  Length of axes
  //  Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glClearColor( .2, .2, .2, 1);
  //  Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  //  Undo previous transformations
  glLoadIdentity();
  //  Perspective - set eye position
  if (mode) {
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    glTranslated(-xpos, -ypos, -zpos);
  }
  //  Orthogonal - set world orientation
  else {
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
    glTranslated(-xpos, -ypos, -zpos);
  }
  //  Flat or smooth shading
  glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

  if (light) {
    float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,.5};
    float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
    float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
    float Position[]  = {sunDistance*Cos(zh),sunElevation,sunDistance*Sin(zh),1.0};
    //  Draw light position as ball (still no lighting here)

    /* The light obviously doesn't come from physically creating the sun object below in the scene.
    Whether we draw this sun object or not is not significant. However, the scene does make more sense if the
     user can see where the light is actively coming from. We acheive this illusion (openGL lighting seemingly 'coming' from our sun)
      by using the idle function to update the physical position of the sun, and then simultanously updating the lighting position as well*/
    if (seeSun)
    sun(Position[0],Position[1],Position[2] , 0.3);

    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
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




  drawDesk(0, -2.5, 0, 3, 3, 3, 0);
  drawDesk(15.5, -2.5, -17, 1, 8, 1, 0);

  glPushMatrix();
  glTranslatef(30, 5, -33);
  drawSides(.3, 0, 0, 0, 0, 1, 1, 1, 0, 0);
  glPopMatrix();
  drawCubeStand(0, 0,0, 1, 1, 1, 0);
  //glTranslated(1.65 , 0, -1.6);
  double cubeSize = 1.5;
  int BW = 1;
  double xLoc = 0;
  double yLoc = 0;
  double zLoc = 0;
  double xSize = 1;
  double ySize = 1;
  double zSize = 1  ;
  double angRot = 45;
  double angElevation = 45;

  glPushMatrix();
  glTranslatef(0, 8, 0);
  glRotated(cubeRotate, 0, 1, 0);
  drawSides(cubeSize, BW, xLoc, yLoc, zLoc, xSize, ySize, zSize, angRot, angElevation);
  glPopMatrix();
  //drawSides(.45, 1, -1, 1.6, 2, 1, 1, 1, 45, 45);

  glColor3f(1,1,1);
  //  Display parameters

  glColor3f(.2,.3,.8);
  glWindowPos2i(5,5);
  Print("Angle=%d,%d  Dim=%.1f FOV=%d xpos=%.3f ypos=%.3f zpos=%.3f",th,ph,dim,fov, xpos, ypos, zpos);
  glFlush();
  glutSwapBuffers();

}



void drawCubeStand(double x,double y,double z, double dx,double dy,double dz, double th) {
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); //Smooth polygons
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Best perspective corrections
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); //Smooth points
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); //Smooth lines
  GLUquadricObj *obj = gluNewQuadric();

  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);


  double cylinderThickness = 2.0;
  double cylinderLength = 1.5;
  glColor3f(.8, .8, .8);
  gluQuadricNormals(obj, GLU_SMOOTH);
  glRotated(-90, 1, 0, 0);
  gluCylinder(obj, cylinderThickness, .3, cylinderLength, 24, 24);
  glPopMatrix();
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
  else glBindTexture(GL_TEXTURE_2D,texture[7]);
  //  Save transformation
  glPushMatrix();
  //  Offset
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx,dy,dz);


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
  glNormal3f(0,1,0);
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

void drawFloor() {

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,textureMode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[5]);
  //  Save transformation
  //glPushMatrix();
  //  Offset
  //glScaled(dx,dy,dz);

  glNormal3d(0,1,0);

  glBegin(GL_QUADS);

  glTexCoord2f(0,0);
  glVertex3f(-Dfloor, YfloorMin, Dfloor);

  glTexCoord2f(1,0);
  glVertex3f(Dfloor, YfloorMin, Dfloor);

  glTexCoord2f(1,1);
  glVertex3f(Dfloor, YfloorMin, -Dfloor);

  glTexCoord2f(0,1);
  glVertex3f(-Dfloor, YfloorMin, -Dfloor);
  glEnd();
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_TEXTURE_2D);
  //  Undo transofrmations
  //glPopMatrix();
}

/*
*  GLUT calls this routine when an arrow key is pressed
*/
void special(int key,int x,int y) {
  //  Right arrow key - increase angle by 5 degrees
  if (key == GLUT_KEY_RIGHT)
  th += 1;
  //  Left arrow key - decrease angle by 5 degrees
  else if (key == GLUT_KEY_LEFT) {
    th -= 1;
  }
  //  Up arrow key - increase elevation by 5 degrees
  else if (key == GLUT_KEY_UP) {
    if (ph < 45) ph += 1;
  }
  //  Down arrow key - decrease elevation by 5 degrees
  else if (key == GLUT_KEY_DOWN) {
    if (ph > -25) ph -= 1;
  }
  //  PageUp key - increase dim
  else if (key == GLUT_KEY_PAGE_UP) {
    if (dim < 12.4) dim += 0.1;
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

void delayTest() {
  printf("callback");

}


void key(unsigned char ch,int x,int y) {

  glLoadIdentity();

  //  Reset everything to starting point
  if (ch == '0') {
    oldth = th;
    newth = 0;

    oldph = ph;
    newph = 0;

    oldxpos = xpos;
    newxpos = -1.545;

    oldypos = ypos;
    newypos = -.97;

    oldzpos = zpos;
    newzpos = 11.573;

    dim = 12.5;
    fov = 63;
    sunSpeed = 1000.0;
    sunDistance = 20;
    sunElevation = 8;
    drawFull = 1;
    drawSkel = 0;
  }

  if (ch == '1') {
    oldth = th;
    newth = 63;
    oldph = ph;
    newph = 22;

    olddim = dim;
    newdim = 9.6;



    oldxpos = xpos;
    newxpos = 36.7;

    oldypos = ypos;
    newypos = .37;

    oldzpos = zpos;
    newzpos = -36.97;
    fov = 55;
  }

  if (ch == '2') {
    oldth = th;
    newth = 62;

    oldph = ph;
    newph = 3;

    oldxpos = xpos;
    newxpos = 3.7;

    oldypos = ypos;
    newypos = 9.5;

    oldzpos = zpos;
    newzpos = -1.637;
    fov = 55;


    olddim = dim;
    newdim = 7.6;

    drawSkel = 1;
    drawFull = 0;
    fov = 63;
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
  // Toggle Skeletons
  else if (ch == 'f')
  drawFull = 1-drawFull;
  else if (ch == 'c')
  seeSun = 1 - seeSun;

  /* Strafing left and right */
  if (ch=='k') {
    float yrotrad;
    yrotrad = (th * (3.141592654f / 180));
    xpos += (float)(cos(yrotrad)) * 0.1;
    zpos += (float)(sin(yrotrad)) * 0.1;
  }

  if (ch=='j') {
    float yrotrad;
    yrotrad = (th * (3.141592654f / 180));
    xpos -= (float)(cos(yrotrad)) * 0.1;
    zpos -= (float)(sin(yrotrad)) * 0.1;
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



  else if (ch=='b') {
    diffuse = 200;
  }
  else if (ch=='B') {
    diffuse = 0;
  }


  //  Reproject
  Project();
  glutIdleFunc(sunMovement?idle:NULL);  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
*  GLUT calls this routine when the window is resized
*/
void reshape(int width,int height) {
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
void idle() {
  //  Elapsed time in seconds
  double t = glutGet(GLUT_ELAPSED_TIME)/sunSpeed;
  if (oldth < newth) {
    th++;
    oldth++;
  }

  if (oldth > newth) {
    th--;
    oldth--;
  }
  if (oldph < newph) {
    ph++;
    oldph++;
  }

  if (oldph > newph) {
    ph--;
    oldph--;
  }

  if (oldxpos < newxpos) {
    xpos++;
    oldxpos++;
  }

  if (oldxpos > newxpos) {
    xpos--;
    oldxpos--;
  }

  if (oldypos < newypos) {
    ypos++;
    oldypos++;
  }

  if (oldypos > newypos) {
    ypos--;
    oldypos--;
  }

  if (oldzpos < newzpos) {
    zpos++;
    oldzpos++;
  }

  if (oldzpos > newzpos) {
    zpos--;
    oldzpos--;
  }

  if (olddim < newdim) {
    dim++;
    olddim++;
  }

  if (olddim > newdim) {
    dim--;
    olddim--;
  }

  zh = fmod(90*t,360.0);
  cubeRotate = fmod(90*t,360.0);
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
*  Start up GLUT and tell it what to do
*/
int main(int argc,char* argv[]) {
    th = 0;
    ph = 0;
    dim = 12.5;
    xpos = -1.545;
    ypos = -.97;
    zpos = 11.573;
    fov = 63;
    sunSpeed = 1000.0;
    sunDistance = 20;
    sunElevation = 8;
    drawFull = 1;
    drawSkel = 0;

//  ypos = 1.861;
//  zpos = -3.842;
  //  Initialize GLUT
  glutInit(&argc,argv);
  //  Request double buffered, true color window with Z buffering at 600x600
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800,800);
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
  texture[3] = LoadTexBMP("woodFloor.bmp");
  texture[4] = LoadTexBMP("popcorn.bmp");
  texture[5] = LoadTexBMP("basketball-floor-texture.bmp");
  texture[6] = LoadTexBMP("chalkboard.bmp");
  texture[7] = LoadTexBMP("brickwall.bmp");
  //  texture[1] = LoadTexBMP("carbon.bmp");
  //  texture[2] = LoadTexBMP("carbon.bmp");
  //  texture[3] = LoadTexBMP("carbon.bmp");
  //  texture[4] = LoadTexBMP("carbon.bmp");
  //  texture[5] = LoadTexBMP("carbon.bmp");
  //  Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
