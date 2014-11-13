/*
 CS 3GC3 Assignment 3
 Stuart Douglas - 1214422
 
 November 9th, 2014
 
 */

#ifndef _TERRAIN_
#define _TERRAIN_
#include "Terrain.h"
#endif

#ifndef _OPENGL_
#define _OPENGL_
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#endif

#ifndef _STANDARD_
#define _STANDARD_
#include <vector>
#include <stdlib.h>
#include <math.h>
#endif

/*****************************************
 *    GLOBAL VARIABLES
 ****************************************/
const float MAX_HEIGHT = 40;


/*****************************************
 * Constructor
 ****************************************/
Terrain::Terrain(int size) {

    //normalize size to acceptable value
    terrainSize = size;
    if (size < 50)
        terrainSize = 50;
    else if (size > MAX_TERRAIN_SIZE)
        terrainSize = MAX_TERRAIN_SIZE;

    //terrain should take up same amount of screen space
    //no matter its size
    scaleFactor = 1.0/((float) terrainSize/100.0);
    
    //make the terrain
    generateTerrain();
}

/*****************************************
 * Initializes heightMap with heights
 * as per line cutting algorithm
 ****************************************/
void Terrain::generateTerrain() {

    //reset heightmap to 0
    for (int x = 0; x < MAX_TERRAIN_SIZE; x++) {
        for (int z = 0; z < MAX_TERRAIN_SIZE; z++) {
            heightMap[x][z] = 3;
        }
    }
    
    //more iterations = more jagged (scales linearly with terrainSize)
    int iterations = terrainSize+250;
    
    for (int i = 0; i < iterations; i++) {
    
        //the following code (until the end of the method) taken from
        //http://www.lighthouse3d.com/opengl/terrain/index.php?impdetails
        //(the website linked in the assignment)
        float v = rand();
        float a = sinf(v);
        float b = cosf(v);
        float d = sqrtf(2*(terrainSize*terrainSize));
        float c = ((double) rand() / RAND_MAX) * d - d/2.0;

        float displacement = 0.8;
        
        //iterate over all points in heightmap
        for (int x = 0; x < terrainSize; x++) {
            for (int z = 0; z < terrainSize; z++) {
                
                //increase the height
                if (a*x + b*z - c > 0)
                    heightMap[x][z] = heightMap[x][z]+displacement < MAX_HEIGHT ? heightMap[x][z] += displacement : MAX_HEIGHT;
                
                //decrease the height
                else
                    heightMap[x][z] = heightMap[x][z]-displacement > 0 ? heightMap[x][z] -= displacement : 0;
            }
            displacement += 0.001;
        }
    }
}

/*****************************************
 * Draws the generated terrain
 ****************************************/
void Terrain::drawTerrain() {

    //scale view
    glScalef(scaleFactor,scaleFactor,scaleFactor);
    
    //iterate over all values in heightmap
    for (int x = 0; x < terrainSize-1; x++) {
        for (int z = 0; z < terrainSize-1; z++) {
            
            //colour is more white w/ more height, green for wireframe
            float colour;
            bool greenColour = false;
            
            //set materials (for lighting)
            float diffuse[4] = {colour,colour,colour, 1.0};
            float ambient[4] = {colour,colour,colour, 1.0};
            float specular[4] = {0.9,0.9,0.9, 1.0};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40);
            
            //set polygon mode, make wireframes bright green
            if (wireframeMode == SOLID) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else if (wireframeMode == WIREFRAME) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                greenColour = true;
            }
            else if (wireframeMode == BOTH) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor3f(0,0.8,0.1);

                glBegin(GL_QUADS);
                    glVertex3f(x, heightMap[x][z], z);
                    glVertex3f(x+1, heightMap[x+1][z], z);
                    glVertex3f(x+1, heightMap[x+1][z+1], z+1);
                    glVertex3f(x, heightMap[x][z+1], z+1);
                glEnd();
                
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            
            //draw the polygons
            glBegin(GL_QUADS);
                colour = (float) heightMap[x][z]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glVertex3f(x, heightMap[x][z], z);
                
                colour = (float) heightMap[x+1][z]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glVertex3f(x+1, heightMap[x+1][z], z);
                
                colour = (float) heightMap[x+1][z+1]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glVertex3f(x+1, heightMap[x+1][z+1], z+1);
                
                colour = (float) heightMap[x][z+1]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glVertex3f(x, heightMap[x][z+1], z+1);
            glEnd();

        }
    }
}


void Terrain::changeWireframeMode() {
    if (wireframeMode == SOLID)
        wireframeMode = WIREFRAME;
    else if (wireframeMode == WIREFRAME)
        wireframeMode = BOTH;
    else if (wireframeMode == BOTH)
        wireframeMode = SOLID;
}

char* Terrain::getWireframeMode() {
    if (wireframeMode == SOLID)
        return (char*) "SOLID";
    else if (wireframeMode == WIREFRAME)
        return (char*) "WIREFRAME";
    else
        return (char*) "BOTH";

}
