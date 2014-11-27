/*
 CS 3GC3 Assignment 3
 Stuart Douglas - 1214422
 Anagh Goswami - 1217426
 November 9th, 2014
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
bool paused = false;

//cam & light positions
float camPos[3] = {-100,60,-100};
float light0Pos[4];
float light1Pos[4];

//character positions & movement vector
float characterPos[3] = {0,40,0};
float characterXDir = 0.1;
float characterZDir = 0.1;

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
    
    //draw scene
    terrain.drawTerrain();
    drawText();
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
    char formatStr[] = "Lighting : %s | Shading: %s | Wireframe: %s | Algorithm: %s | Size: %d x %d";
    char outputStr[100];
    
    //for some reason sprintf requires the _s in windows
    #ifdef __APPLE__
        sprintf(outputStr, formatStr,(lighting ? "ON" : "OFF"),(gouraudShading ? "GOURAUD" : "FLAT"),terrain.getWireframeMode(),terrain.getAlgorithm(), terrain.terrainSize,terrain.terrainSize);
    #else
        sprintf_s(outputStr, formatStr,(lighting ? "ON" : "OFF"),(gouraudShading ? "GOURAUD" : "FLAT"),terrain.getWireframeMode(),terrain.getAlgorithm(),terrain.terrainSize,terrain.terrainSize);
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

/****************************************
* moves character in x and y directions,
* turning around if hits edge of terrain
****************************************/
void moveCharacter() {
    
    float terrainOffset = terrain.terrainSize/2.0;
    
    //x is within terrain
    if (characterPos[0]+characterXDir <= terrainOffset-1 && characterPos[0]+characterXDir >= -terrainOffset+1)
        characterPos[0] += characterXDir;
    //x hit bound
    else {
        characterXDir = -characterXDir;
        characterPos[0] += characterXDir;
    }
    
    //z within terrain
    if (characterPos[2]+characterZDir <= terrainOffset-1 && characterPos[2]+characterZDir >= -terrainOffset+1)
        characterPos[2] += characterZDir;
    //z hit bound
    else {
        characterZDir = -characterZDir;
        characterPos[2] += characterZDir;
    }

    //we didn't do full bilinear interpolation, instead we took two opposite points of
    //the quad (a and b) and interpolated our current height from the distance
    //down that line. This makes his height movement a little less jumpy
    float xIndexInHeightmap = characterPos[0] + terrain.terrainSize/2.0;
    float zIndexInHeightmap = characterPos[2] + terrain.terrainSize/2.0;
    
    float aHeight = terrain.heightMap[(int)floor(xIndexInHeightmap)][(int)floor(zIndexInHeightmap)];
    float bHeight = terrain.heightMap[(int)floor(xIndexInHeightmap+1)][(int)floor(zIndexInHeightmap+1)];

    float xPercent = xIndexInHeightmap-floor(xIndexInHeightmap);
    float zPercent = zIndexInHeightmap-floor(zIndexInHeightmap);
    float distOnABLine = sqrtf(xPercent*xPercent + zPercent*zPercent);
    
    characterPos[1] = aHeight+distOnABLine*(bHeight-aHeight);;
}

/****************************************
* draws a snowman in characterPos
* code taken from Snowman.c file uploaded
* to Avenue by Dr. Teather
****************************************/
void drawSnowman() {
    
    //set initial colour to white
    glColor3f(1,1,1);
    float diffuse[4] = {1,1,1, 1};
    float diffuseBlack[4] = {0,0,0, 1};
    float diffuseOrange[4] = {1,0.4,0, 1};
    float ambient[4] = {1,1,1, 1};
    float ambientBlack[4] = {0,0,0, 1};
    float ambientOrange[4] = {1,0.4,0, 1};
    float specular[4] = {0.1,0.1,0.1, 0.5};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2);
    
    //body is white
    glPushMatrix();
    glTranslatef(characterPos[0], characterPos[1], characterPos[2]);
    glutSolidSphere(1, 16, 16); //body
    
    //buttons are black
    glColor3f(0, 0, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBlack);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientBlack);

    glPushMatrix();
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 0.15, 0.95);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, -0.05, 0.95);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();

    //head is white
    glColor3f(1,1,1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

    glPushMatrix();
    glTranslatef(0, 1.25, 0);
    glutSolidSphere(0.5, 16, 16);
    
    //eyes are black
    glColor3f(0,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBlack);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientBlack);

    glPushMatrix();
    glTranslatef(0.2, 0.15, 0.45);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.2, 0.15, 0.45);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    //nose is orange
    glColor3f(1,0.4,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseOrange);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientOrange);
    
    glPushMatrix();
    glTranslatef(0, 0, 0.5);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    
    glPopMatrix(); //body
    glPopMatrix(); //snowman
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
			camPos[2] -= (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
        case ']':
			camPos[2] += (2 * terrain.terrainSize) / terrain.terrainSize;
            break;

        //change algorithms
        case '1':
            terrain.changeTerrainAlgorithm(Terrain::FAULT);
            break;
        case '2':
            terrain.changeTerrainAlgorithm(Terrain::CIRCLE);
            break;
            
        //reset scene, character, and lights
        case 'r':
        case 'R':
            terrain.generateTerrain();
            characterPos[0] = 0;
            characterPos[2] = 0;
            characterXDir = ((double) rand() / RAND_MAX)/2.0;
            characterZDir = ((double) rand() / RAND_MAX)/2.0;
            light0Pos[0] = terrain.terrainSize/2.0;
            light0Pos[2] = terrain.terrainSize/2.0;
            light1Pos[0] = -terrain.terrainSize/2.0;
            light1Pos[2] = -terrain.terrainSize/2.0;
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
            
        //rotate lights in circle
        case ',':
        case '<':
            light0Pos[0] -= (2*terrain.terrainSize)/terrain.terrainSize;
			light1Pos[0] -= (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
        case '.':
        case '>':
			light0Pos[2] -= (2 * terrain.terrainSize) / terrain.terrainSize;
			light1Pos[2] -= (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
        case '/':
        case '?':
			light0Pos[0] += (2 * terrain.terrainSize) / terrain.terrainSize;
			light1Pos[0] += (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
        case ';':
        case ':':
			light0Pos[2] += (2 * terrain.terrainSize) / terrain.terrainSize;
			light1Pos[2] += (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
        
        //pause
        case 'p':
        case 'P':
            paused = !paused;
            break;
            
        //quitting
        case 'q':
        case 27:
            exit (0);
            break;
    }
    //move lights
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glPopMatrix();
    
    glutPostRedisplay();
}

/*****************************************
* handles arrow key presses (to move cam)
****************************************/
void special(int key, int x, int y) {
    
    //move camera w/ arrow keys
    switch(key) {
        case GLUT_KEY_LEFT:
			camPos[0] -= (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
            
        case GLUT_KEY_RIGHT:
			camPos[0] += (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
            
        case GLUT_KEY_UP:
			camPos[1] += (2 * terrain.terrainSize) / terrain.terrainSize;
            break;
            
        case GLUT_KEY_DOWN:
			camPos[1] -= (2 * terrain.terrainSize) / terrain.terrainSize;
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
    printf("Enter terrain size (recommended between 50 and 300 for best output):\n");
    scanf("%d",&terrainSize);

    //introduction console text
    printf("\nWelcome to CS 3GC3 Assignment 3 - Terrain Generator!");
    printf("\nStuart Douglas - 1214422 | Anagh Goswami - 1217426");
    printf("\n\nThe following keyboard functions can be used:");
    printf("\n\t'1': LINE-FAULT algorithm\n\t'2': CIRCLE algorithm");
    printf("\n\t'q': Quit Game\n\t'p': Pause Character\n\t'r': Generate a new terrain, reset lights and character");
    printf("\n\t'w': Toggle Wireframe Modes");
    printf("\n\t's': Toggle Gouraud or Flat shading");
    printf("\n\t'l': Toggle Lighting on or off");
	printf("\n\t'Up and Down Arrow Keys': Move Camera Up and Down");
    printf("\n\t'Left and Right Arrow Keys': Move Camera on Z axis");
	printf("\n\t'[' and ']': Move Camera on X axis");
    printf("\n\t'<' Move Lights in -X direction");
    printf("\n\t'/' Move Lights in +X direction");
    printf("\n\t'.' Move Lights in -Z direction");
    printf("\n\t';' Move Lights in +Z direction");
    printf("\nBonus Features:");
    printf("\n\tState Text: current values of user-definable state shown in bottom left");
    printf("\n\tMoving Character: A snowman moves around the terrain, adjusting to the height of the terrain and turning around when he hits the edges.");
    printf("\n\tMultiple Algorithms: 2 Algorithms for generating the terrian are included (press 1 and 2 to switch).");
    printf("\n\tTerrain Colours: Depending on the height, terrain is blue (water), brown (land), green (grass), or white (snow).");
    printf("\n\tSmooth Terrain: We have implemented a smoothing algorithm for the FAULT line terrain that makes it less jagged.");

    //initialize terrain
    terrain = Terrain(terrainSize);
    
    characterXDir = ((double) rand() / RAND_MAX)/2.0;
    characterZDir = ((double) rand() / RAND_MAX)/2.0;
    
    //put lights on opposite sides of terrain
    light0Pos[0] = (float) terrain.terrainSize/2.0;
    light0Pos[1] = 80;
    light0Pos[2] = (float) terrain.terrainSize/2.0;
    light0Pos[3] = 1.0;
    
    light1Pos[0] = -(float) terrain.terrainSize/2.0;
    light1Pos[1] = 80;
    light1Pos[2] = -(float) terrain.terrainSize/2.0;
    light1Pos[3] = 1.0;
    
    //set camera pos
    camPos[0] = -terrain.terrainSize + terrain.terrainSize/4;
    camPos[2] = -terrain.terrainSize + terrain.terrainSize/4;
    
    //set backrgound to dark gray
    glClearColor(0.25, 0.25, 0.25, 1);

    //turn on lighting & back-face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

    //set projection matrix, using perspective w/ correct aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(GLfloat) glutGet(GLUT_WINDOW_WIDTH) / (GLfloat) glutGet(GLUT_WINDOW_HEIGHT), 1, 100);
}

/****************************************
* moves snowman around landscape
****************************************/
void timerFunc(int value) {
    if (!paused)
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