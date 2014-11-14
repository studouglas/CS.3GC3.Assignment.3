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
 *    GLOBAL VARIABLES
 ****************************************/
const float MAX_HEIGHT = 40;
float vertexNormals[MAX_TERRAIN_SIZE][MAX_TERRAIN_SIZE][3];
float faceNormals[MAX_TERRAIN_SIZE][MAX_TERRAIN_SIZE][3];

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
    
    //make the terrain
    generateTerrain();
}

/*****************************************
 * Initializes heightMap with heights
 * as per line cutting algorithm
 ****************************************/
void Terrain::generateTerrain() {

    //reset heightmap
    for (int x = 0; x < MAX_TERRAIN_SIZE; x++) {
        for (int z = 0; z < MAX_TERRAIN_SIZE; z++) {
            heightMap[x][z] = 10;
        }
    }
    
    //more iterations = more jagged (scales linearly with terrainSize)
    int iterations = terrainSize+250;
    
    for (int i = 0; i < iterations; i++) {
    
        //the following code (until the end of the method) taken from
        //www.lighthouse3d.com/opengl/terrain/index.php?impdetails
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

        }
        displacement += 0.01;
    }
    calculateVertexNormals();
    calculateFaceNormals();
}

/*****************************************
 * Draws the generated terrain
 ****************************************/
void Terrain::drawTerrain() {
    
    //set materials for land, snow and water
    float diffuse[4] = {0,0.4,0, 1.0};
    float ambient[4] = {0,0.4,0, 1.0};
    float specular[4] = {0.1,0.1,0.1, 0.5};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2);
    
    //iterate over all values in heightmap
    for (int x = 0; x < terrainSize-1; x++) {
        for (int z = 0; z < terrainSize-1; z++) {
            
            //colour is more white w/ more height, green for wireframe
            float colour;
            bool drawingWireframe = false;
            
            //set snow and water materials
            if (heightMap[x][z] <= 4) {
                float diffuseWater[4] = {0,0,0.8, 1};
                float ambientWater[4] = {0,0,0.8, 1};
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientWater);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseWater);
            }
            else if (heightMap[x][z] >= 21) {
                float diffuseSnow[4] = {1,1,1, 1};
                float ambientSnow[4] = {1,1,1, 1};
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientSnow);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseSnow);
            }

            //set polygon mode, make wireframes bright green if no lighting
            if (wireframeMode == SOLID) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else if (wireframeMode == WIREFRAME) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                drawingWireframe = true;
            }
            else if (wireframeMode == BOTH) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                drawingWireframe = true;
            }
            
            glNormal3fv(faceNormals[x][z]);
            
            //draw the quad (twice if both is wireframe mode)
            for (int i = 0; i < 2; i++) {
                
                glBegin(GL_QUADS);
                    colour = (float) heightMap[x][z]/ (float) MAX_HEIGHT;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x][z]);
                    glVertex3f(x, heightMap[x][z], z);
                
                    colour = (float) heightMap[x][z+1]/ (float) MAX_HEIGHT;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x][z+1]);
                    glVertex3f(x, heightMap[x][z+1], z+1);
                    
                    colour = (float) heightMap[x+1][z+1]/ (float) MAX_HEIGHT;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x+1][z+1]);
                    glVertex3f(x+1, heightMap[x+1][z+1], z+1);
             
                    colour = (float) heightMap[x+1][z]/ (float) MAX_HEIGHT;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x+1][z]);
                    glVertex3f(x+1, heightMap[x+1][z], z);
                glEnd();
                
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                drawingWireframe = false;
                if (wireframeMode != BOTH)
                    break;
            }
            
            //reset material if snow or water just drawn
            if (heightMap[x][z] <= 4 || heightMap[x][z] >= 21) {
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            }
        }
    }
}

/**************************************************************
 * calculates normals for every vertex in the heightmap. 
 * Code was taken from:
 * www.lighthouse3d.com/opengl/terrain/index.php3?normals
 * We modified the code to suit our needs.
 **************************************************************/
void Terrain::calculateVertexNormals() {
    
    //calculate normals
    for (int x = 0; x < terrainSize; x++) {
        for (int z = 0; z < terrainSize; z++) {
            
            //x, z
            float t1[3];
            t1[0] = x; t1[1] = heightMap[x][z]; t1[2] = z;
            
            //x+1, z
            float t3[3];
            t3[0] = x+1; t3[1] = heightMap[x+1][z]; t3[2] = z;
            
            //x, z+1
            float t2[3];
            t2[0] = x; t2[1] = heightMap[x][z+1]; t2[2] = z+1;
            
            float v1[3] = {t2[0]-t1[0], t2[1]-t1[1], t2[2]-t1[2]};
            float v2[3] = {t3[0]-t1[0], t3[1]-t1[1], t3[2]-t1[2]};
            
            float vx = v1[1]*v2[2] - v1[2]*v2[1];
            float vy = v1[2]*v2[0] - v1[0]*v2[2];
            float vz = v1[0]*v2[1] - v1[1]*v2[0];
            
            float len = sqrtf(vx*vx + vy*vy + vz*vz);
            float nv[3] = {vx/len, vy/len, vz/len};
            
            vertexNormals[x][z][0] = nv[0];
            vertexNormals[x][z][1] = nv[1];
            vertexNormals[x][z][2] = nv[2];
        }
    }
}

/**************************************************************
 * calculates normals for every FACE in the heightmap. The indice
 * of the bottom corner (x=0,z=0 for bottom left quad) of each 
 * quad holds the face normal of that quad in the faceNormals array.
 * www.lighthouse3d.com/opengl/terrain/index.php3?normals was
 * used as a reference. Their code is for calculating vertex normals
 * from face normals, we calculated faces by normalizing vertex ones.
 **************************************************************/
void Terrain::calculateFaceNormals() {

    //iterate over all values in heightmap except right most
    //and topmost vertices (since bottom left holds faceNormal)
    for (int x = 0; x < terrainSize-1; x++) {
        for (int z = 0; z < terrainSize-1; z++) {
            
            //x, z
            float v1[3];
            v1[0] = vertexNormals[x][z][0];
            v1[1] = vertexNormals[x][z][1];
            v1[2] = vertexNormals[x][z][2];
            
            //x+1, z
            float v2[3];
            v2[0] = vertexNormals[x+1][z][0];
            v2[1] = vertexNormals[x+1][z][1];
            v2[2] = vertexNormals[x+1][z][2];
            
            //x+1, z+1
            float v3[3];
            v3[0] = vertexNormals[x+1][z+1][0];
            v3[1] = vertexNormals[x+1][z+1][1];
            v3[2] = vertexNormals[x+1][z+1][2];

            //x, z+1
            float v4[3];
            v4[0] = vertexNormals[x][z+1][0];
            v4[1] = vertexNormals[x][z+1][1];
            v4[2] = vertexNormals[x][z+1][2];
            
            //normalize the 3 surrounding vertex normals
            faceNormals[x][z][0] = (v1[0]+v2[0]+v3[0]+v4[0])/4.0;
            faceNormals[x][z][1] = (v1[1]+v2[1]+v3[1]+v4[1])/4.0;
            faceNormals[x][z][2] = (v1[2]+v2[2]+v3[2]+v4[2])/4.0;
        }
    }
}

/*****************************************
* Toggles through the 3 wireframe modes.
*****************************************/
void Terrain::changeWireframeMode() {
    if (wireframeMode == SOLID)
        wireframeMode = WIREFRAME;
    else if (wireframeMode == WIREFRAME)
        wireframeMode = BOTH;
    else if (wireframeMode == BOTH)
        wireframeMode = SOLID;
}

/*****************************************
* Returns string of wireframe mode
****************************************/
char* Terrain::getWireframeMode() {
    if (wireframeMode == SOLID)
        return (char*) "SOLID";
    else if (wireframeMode == WIREFRAME)
        return (char*) "WIREFRAME";
    else
        return (char*) "BOTH";

}
