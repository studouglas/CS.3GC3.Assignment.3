/*
 CS 3GC3 Assignment 3
 Stuart Douglas - 1214422
 Anagh Goswami - 1217426
 November 9th, 2014
 
 main.cpp
 -
*/

#include "Terrain.h"

#include <vector>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <windows.h>
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif


/*****************************************
 *    FUNCTION DECLARATIONS
 ****************************************/
void drawText();
void moveCharacter();
void drawSnowman();

/*****************************************
 *    GLOBAL VARIABLES
 ****************************************/
Terrain terrain = NULL;

//state (modified by key presses)
bool lighting = true;
bool gouraudShading = true;

//camera (modified by arrow keys)
float camPos[3] = {-100,60,-100};

//light position (modified by WASD & TG)
float light1Pos[4];
float light2Pos[4];

float position[3] = {0,40,0};
//float rot[3] = {0, 0, 0};
float headRot[] = {0, 0, 0};
float angle = 0.0f;
float xDir = 0.1;
float zDir = 0.1;

/*****************************************
 * displays all objects
 ****************************************/
void display(void) {
    
    //clear bits and model view matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //point camera
    gluLookAt(camPos[0],camPos[1],camPos[2], 0,0,0, 0,1,0);
    
    drawText();
//    glDisable(GL_LIGHTING);
//    glColor3f(1, 0, 0);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(300, 0, 0);
//    glEnd();
//    glColor3f(0, 1, 0);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(0, 300, 0);
//    glEnd();
//    glColor3f(0, 0, 1);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(0, 0, 300);
//    glEnd();
//    if (lighting)
//        glEnable(GL_LIGHTING);
//    
    terrain.drawTerrain();
    drawSnowman();
    glutSwapBuffers();
}

/*****************************************
 * draws current states of user-modifiable
 * variables in text in bottom left
 ****************************************/
void drawText() {
    
    //clear matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    //change projection to gluOrtho2D temporarily (text drawn in 2d)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    
    //disable lighting (text shouldn't be affected by light)
    glDisable(GL_LIGHTING);
    
    //set up string to print
    char formatStr[] = "Lighting : %s | Shading: %s | Wireframe : %s";
    char outputStr[100];
    
    //for some reason sprintf requires the _s in windows
    #ifdef __APPLE__
        sprintf(outputStr, formatStr,(lighting ? "ON" : "OFF"),(gouraudShading ? "GOURAUD" : "FLAT"),terrain.getWireframeMode());
    #else
        sprintf_s(outputStr, formatStr,(lighting ? "ON" : "OFF"),(gouraudShading ? "GOURAUD" : "FLAT"),terrain.getWireframeMode());
    #endif

    //display string
    glColor3f(1, 1, 1);
    glRasterPos2f(10,10);
    for (int i = 0; i < strlen(outputStr); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, outputStr[i]);
    
    //revert previous matrices
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    if (lighting)
        glEnable(GL_LIGHTING);
}

void moveCharacter() {
    
    float terrainOffset = terrain.terrainSize/2.0;
    
    if (position[0]+xDir <= terrainOffset-0.5 && position[0]+xDir >= -terrainOffset+0.5) {
        position[0] += xDir;
    }
    else {
        xDir = -xDir;
        position[0] += xDir;
    }
    
    if (position[2]+zDir <= terrainOffset-0.5 && position[2]+zDir >= -terrainOffset+0.5) {
        position[2] += zDir;
    }
    else {
        zDir = -zDir;
        position[2] += zDir;
    }

    int x = (int) position[0] + terrain.terrainSize/2.0;
    int z = (int) position[2] + terrain.terrainSize/2.0;
    
    //^  B ---- C
    //|  |      |
    //z  A ---- D
    //x ---->
    float A = terrain.heightMap[(int)floor(x)][(int)floor(z)];
    //float B = terrain.heightMap[(int)floor(x)][(int)ceil(z)];
    float C = terrain.heightMap[(int)ceil(x)][(int)ceil(z)];
    //float D = terrain.heightMap[(int)ceil(x)][(int)floor(z)];

    float xPercent = x-floor(x);
    float zPercent = z-floor(z);
    float a = sqrtf(xPercent*xPercent + zPercent*zPercent);
    
    float b = A+a*(C-A);
    
    position[1] = b;

}

void drawSnowman() {
    
    float diffuse[4] = {1,1,1, 1};
    float ambient[4] = {1,1,1, 1};
    float specular[4] = {0.1,0.1,0.1, 0.5};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2);
    
    glPushMatrix();
    
    glTranslatef(position[0], position[1], position[2]);
    
    //draw body
    glColor3f(1,1,1);
    glutSolidSphere(1, 16, 16);
    
    //draw buttons
    glPushMatrix();
    glTranslatef(0, 0.35, 0.9);
    glColor3f(0, 0, 0);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 0.15, 0.95);
    glColor3f(0, 0, 0);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, -0.05, 0.95);
    glColor3f(0, 0, 0);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    
    //translate relative to body, and draw head
    glTranslatef(0, 1.25, 0);
    glRotatef(headRot[1], 0, 1, 0); //turn the head relative to the body
    glColor3f(1,1,1);
    glutSolidSphere(0.5, 16, 16);
    
    //translate and draw right eye
    glPushMatrix();
    glTranslatef(0.2, 0.15, 0.45);
    glColor3f(0,0,0);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    //translate and draw left eye
    glPushMatrix();
    glTranslatef(-0.2, 0.15, 0.45);
    glColor3f(0,0,0);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    //translate and draw nose
    glPushMatrix();
    glTranslatef(0, 0, 0.5);
    glColor3f(1,0.4,0);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPopMatrix();//body
    glPopMatrix();//snowman
}


/********************************************
 * handles key presses for program functions
 *******************************************/
void keyboard(unsigned char key, int x, int y) {
    
    switch (key) {
           
        //lighting
        case 'l':
        case 'L':
            lighting = !lighting;
            if (lighting) {
                glEnable(GL_LIGHTING);
                glEnable(GL_CULL_FACE);
            }
            else {
                glDisable(GL_LIGHTING);
                glDisable(GL_CULL_FACE);
            }
            break;
            
        //move camera
        case '[':
            camPos[2] -= 1;
            break;
        case ']':
            camPos[2] += 1;
            break;

        //change algorithms
        case '1':
            terrain.changeTerrainAlgorithm(Terrain::FAULT);
            break;
        case '2':
            terrain.changeTerrainAlgorithm(Terrain::CIRCLE);
            break;
            
        //reset
        case 'r':
        case 'R':
            terrain.generateTerrain();
            position[0] = 0;
            position[2] = 0;
            xDir = ((double) rand() / RAND_MAX)/2.0;
            zDir = ((double) rand() / RAND_MAX)/2.0;
            break;

        //toggle shading
        case 's':
        case 'S':
            gouraudShading = !gouraudShading;
            if (gouraudShading)
                glShadeModel(GL_SMOOTH);
            else
                glShadeModel(GL_FLAT);
            break;
            
        //toggle through wireframe modes
        case 'w':
        case 'W':
            terrain.changeWireframeMode();
            break;
            
        //quitting
        case 'q':
        case 27:
            exit (0);
            break;
    }
    glutPostRedisplay();
}

/*****************************************
 * handles arrow key presses (to move cam)
 ****************************************/
void special(int key, int x, int y) {
    
    //move camera w/ arrow keys
    switch(key) {
        case GLUT_KEY_LEFT:
            camPos[0] -= 1;
            break;
            
        case GLUT_KEY_RIGHT:
            camPos[0] += 1;
            break;
            
        case GLUT_KEY_UP:
            camPos[1] += 1;
            break;
            
        case GLUT_KEY_DOWN:
            camPos[1] -= 1;
            break;
    }
    glutPostRedisplay();
}

/********************************************
* sets viewport according to window size
*******************************************/
void reshapeFunc(int w, int h) {
    
    //don't let window become less than 300 x 300
    int minWindowSize = 300;
    if (w < minWindowSize || h < minWindowSize)
        glutReshapeWindow((w < minWindowSize) ? minWindowSize : w, (h < minWindowSize) ? minWindowSize : h);
    
    else {
        //set projection matrix, using perspective
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        //set up viewport
        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        gluPerspective(45, (GLfloat) w / (GLfloat) h, 1, 500);
    }
    
    glutPostRedisplay();
}

/*******************************************
 *initializes global variables and settings
 ******************************************/
void init() {
    
    //get terrain size
    int terrainSize = 100;
    printf("Enter terrain size (min 50, max 300):\n");
//    scanf("%d",&terrainSize);

    //initialize terrain
    terrain = Terrain(terrainSize);
    
    xDir = ((double) rand() / RAND_MAX)/2.0;
    zDir = ((double) rand() / RAND_MAX)/2.0;
    
    //put light 1 in middle of terrain
    light1Pos[0] = (float) terrain.terrainSize/2.0;
    light1Pos[1] = 80;
    light1Pos[2] = (float) terrain.terrainSize/2.0;
    light1Pos[3] = 1.0;
    
    //put light 2 at origin
    light2Pos[0] = 0;
    light2Pos[1] = 80;
    light2Pos[2] = 0;
    light2Pos[3] = 1.0;
    
    //set camera pos
    camPos[0] = -terrain.terrainSize + terrain.terrainSize/4;
    camPos[2] = -terrain.terrainSize + terrain.terrainSize/4;
    
    //set backrgound to dark gray
    glClearColor(0.25, 0.25, 0.25, 1);

    //turn on lighting & back-face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light1Pos);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light2Pos);

    //set projection matrix, using perspective w/ correct aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(GLfloat) glutGet(GLUT_WINDOW_WIDTH) / (GLfloat) glutGet(GLUT_WINDOW_HEIGHT), 1, 100);
}

void timerFunc(int value) {
    moveCharacter();
    
    glutTimerFunc(32, timerFunc, 0);
    glutPostRedisplay();
}

/****************************************
* program start point 
****************************************/
int main(int argc, char** argv) {
    
    //initializeing GLUT
    glutInit(&argc, argv);
    
    //making our window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Terrain Generator");

    //initializing variables
    init();
    
    //registering callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(32, timerFunc, 0);
    glutReshapeFunc(reshapeFunc);
    
    //setting up depth test
    glEnable(GL_DEPTH_TEST);
    
    //start event loop
    glutMainLoop();
    
    return(0);
}