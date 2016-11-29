#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>


//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



/* LIGHT NOTES */
/*
 * Ambient Light:
 * Ambient light alone can't completely illustrate a 3D object in space because
 * all vertices are evenly lit by the same color and the object appears to be 2D
 * Average volume of light created by all the light sources surrounding the lit area
 *
 * Diffuse Light:
 * Diffuse light represents a directional light cast by a light source. This kind of light has a position in space and comes from a single direction. When diffuse light touches the surface of an object, it scatters and reflects evenly across the surface
 *
 * Specular Light:
 * The smoother the object, the more defined the specular highlight will be.
 * Specular light is the shiny spot that can be sit on a sphere right where the light is hitting.
 * Directional type of light, only comes from one spot. Specular light (unlike diffuse light), reflects off the surface in a sharp and uniform way. Rendering of specular light depends on the angle between the viewer and the light source. The nature of this specular light can give insight into the material (plastic = super shiny vs. leather = less shiny).
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

int axes=0;       //  Display axes
int textureMode  = 0;
int mode=1;       //  Projection mode
int th=25;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=6.0;   //  Size of world

double sunSpeed = 1000.0;
int sunDistance = 5;
float sunElevation = 1;
int sunMovement = 1;
int seeSun = 1;


double spacing = .1;
double squareSize = .2;

float xpos, ypos, zpos = 0;

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  1;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
/* Phong Lighting Model: Based on Ambient, Diffuse, Specular */
int ambient   =  18;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%) Seems to be how 'bright' the sun is. A value of 0 results in no light
int specular  =   0;  // Specular intensity (%)
int emission  =   0;  // Emission intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
int light = 1;


//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))


#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...);
static void Project();
void cubeSide(double squareSize);
void drawSkeleton();
void drawSides(double squareSize, int bw);
static void square(double x,double y,double z, double dx,double dy,double dz, double th);
void drawDesk();
void drawTableTop(double x, double y, double z, double dx, double dy, double dz, double th);
void drawTableLeg(double x, double y, double z, double dx, double dy, double dz, double th);
void drawTableLegSide(double x, double y, double z, double dx, double dy, double dz, double th);
void drawFloor();
void drawCeiling();
void drawWalls(double x,double y,double z, double dx,double dy,double dz, double th);



/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display();

void resetMatrix();



void special(int key,int x,int y);


/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y);


/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height);



void idle();
/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[]);
