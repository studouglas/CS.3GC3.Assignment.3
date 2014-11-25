/*
 CS 3GC3 Assignment 3
 Stuart Douglas - 1214422
 Anagh Goswami - 1217426
 November 10th, 2014
 
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
float maxTerrainHeightValue = 0;

/*****************************************
 * Constructor
 ****************************************/
Terrain::Terrain(int size) {

    //normalize size to acceptable value
    if (size < 50)
        terrainSize = 50;
    if (size > MAX_TERRAIN_SIZE)
        terrainSize = MAX_TERRAIN_SIZE;
    else
        terrainSize = size;
    
    //make the terrain
    generateTerrain();
}

/*****************************************
 * Initializes heightMap with heights as
 * per line cutting & circles algorithms
 ****************************************/
void Terrain::generateTerrain() {

    //reset heightmap
    maxTerrainHeightValue = 0;
    for (int x = 0; x < MAX_TERRAIN_SIZE; x++) {
        for (int z = 0; z < MAX_TERRAIN_SIZE; z++) {
            heightMap[x][z] = 0;
        }
    }

    //the following algorithms use code from
    //www.lighthouse3d.com/opengl/terrain/index.php
    //(the website linked in the assignment)
    
    if (terrainAlgorithm == CIRCLE) {
        
        //these values all depend on the terrain size (big terrain needs more circles)
        int iterations = terrainSize*3 + 600;
        float displacement = 2 + 3*(terrainSize/100.0);
        float terrainCircleRadius = 10 + 5*(terrainSize/100.0);

        //choose a circle, raise points in it
        for (int i = 0; i < iterations; i++) {
            
            //choose random point
            float randX = ((double) rand() / RAND_MAX) * terrainSize;
            float randZ = ((double) rand() / RAND_MAX) * terrainSize;
            
            //check all points in circle, raise them appropriately
            for (int x = randX-terrainCircleRadius; x <= randX+terrainCircleRadius; x++) {
                for (int z = randZ-terrainCircleRadius; z <= randZ+terrainCircleRadius; z++) {
                    
                    //if our point in terrain, raise it
                    if (x >= 0 && x < terrainSize && z >= 0 && z < terrainSize) {

                        //determine distance to circle w/ distance algorithm
                        float pointDist = sqrtf(((x-randX)*(x-randX) + (z-randZ)*(z-randZ)))*2.0/terrainCircleRadius;

                        if (fabs(pointDist) <= 1.0)
                            heightMap[x][z] += displacement/2.0 + cosf(pointDist*3.14)*displacement/2.0;
                        
                        if (heightMap[x][z] > maxTerrainHeightValue)
                            maxTerrainHeightValue = heightMap[x][z];
                    }
                }
            }
            displacement = displacement >= 0.06 ? displacement-0.01 : 0.05;
        }
    }

    //line fault algorithm
    else {

        int iterations = (terrainSize*terrainSize*terrainSize)/10000+200, v;
        float displacement = 1, a, b, c, d;

        for (int i = 0; i < iterations; i++) {
            
            //choose random line
            v = rand();
            a = sinf(v);
            b = cosf(v);
            d = sqrtf(2.0*(terrainSize*terrainSize));
            c = ((double) rand()/RAND_MAX) * d - d/2.0;
            
            //iterate over all points in heightmap
            for (int x = 0; x < terrainSize; x++) {
                for (int z = 0; z < terrainSize; z++) {

                    //increase the height
                    if (a*x + b*z - c > 0) {
                        heightMap[x][z] = heightMap[x][z]+displacement < MAX_HEIGHT ? heightMap[x][z] += displacement : MAX_HEIGHT;
                        if (heightMap[x][z] > maxTerrainHeightValue)
                            maxTerrainHeightValue = heightMap[x][z];
                    }
                    
                    //decrease the height
                    else
                        heightMap[x][z] = heightMap[x][z]-displacement > 0 ? heightMap[x][z] -= displacement : 0;
                }

            }
            displacement = displacement > 0.2 ? displacement-0.001 : 0.2;
        }
        smoothTerrain(0.5);
    }
    
    calculateVertexNormals();
    calculateFaceNormals();
}


/*****************************************
 * Smooths out the rough terrain using
 * algorithm from:
 * www.lighthouse3d.com/opengl/terrain/index.php3?smoothing
 ****************************************/
void Terrain::smoothTerrain(float smooth) {

    //rows, left to right
    for (int x = 1; x < terrainSize; x++)
        for (int z = 0; z < terrainSize; z++)
            heightMap[x][z] = heightMap[x-1][z]*smooth + heightMap[x][z]*(1-smooth);

    //rows, right to left
    for (int x = terrainSize-2; x > -1; x--)
        for (int z = 0; z < terrainSize; z++)
            heightMap[x][z] = heightMap[x+1][z]*smooth + heightMap[x][z]*(1-smooth);

    //columns, bottom to top
    for (int x = 0; x < terrainSize; x++)
        for (int z = 1; z < terrainSize; z++)
            heightMap[x][z] = heightMap[x][z-1]*smooth + heightMap[x][z]*(1-smooth);

    //columns, top to bottom
    for (int x = 0; x < terrainSize; x++)
        for (int z = terrainSize; z > -1; z--)
            heightMap[x][z] = heightMap[x][z+1]*smooth + heightMap[x][z]*(1-smooth);

}

/*****************************************
 * Draws the generated terrain
 ****************************************/
void Terrain::drawTerrain() {
    
    float specular[4] = {0.1,0.1,0.1, 0.5};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2);
    
    //iterate over all values in heightmap
    for (int x = 0; x < terrainSize-1; x++) {
        for (int z = 0; z < terrainSize-1; z++) {
            
            //colour is more white w/ more height, green for wireframe
            float colour;
            bool drawingWireframe = false;
            
            //set snow and water materials (it looks dumb with the circle algorithm so we disable it)
            if (heightMap[x][z] <= 4){// && terrainAlgorithm != CIRCLE) {
                float diffuseWater[4] = {0,0,0.8, 1};
                float ambientWater[4] = {0,0,0.8, 1};
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientWater);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseWater);
            }
            else if (heightMap[x][z] > 4 && heightMap[x][z] <= 8 ){//&& terrainAlgorithm != CIRCLE) {
                float diffuseGrass[4] = {0,0.3,0.2, 1};
                float ambientGrass[4] = {0, 0.3, 0.2, 1};
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientGrass);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseGrass);
            }
            else if (heightMap[x][z] >= 21 ){//&& terrainAlgorithm != CIRCLE) {
                float diffuseSnow[4] = {1,1,1, 1};
                float ambientSnow[4] = {1,1,1, 1};
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientSnow);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseSnow);
            }
            else {
                float diffuseDefault[4] = {0.52,0.26,0.08, 1.0};
                float ambientDefault[4] = {0.52,0.26,0.08, 1.0};
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
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
            float terrainOffset = terrainSize/2.0;
            
            //draw the quad (twice if both is wireframe mode)
            for (int i = 0; i < 2; i++) {
                
                glBegin(GL_QUADS);
                    colour = (float) heightMap[x][z]/ (float) maxTerrainHeightValue;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x][z]);
                    glVertex3f(x-terrainOffset, heightMap[x][z], z-terrainOffset);
                
                    colour = (float) heightMap[x][z+1]/ (float) maxTerrainHeightValue;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x][z+1]);
                    glVertex3f(x-terrainOffset, heightMap[x][z+1], z+1-terrainOffset);
                    
                    colour = (float) heightMap[x+1][z+1]/ (float) maxTerrainHeightValue;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x+1][z+1]);
                    glVertex3f(x+1-terrainOffset, heightMap[x+1][z+1], z+1-terrainOffset);
             
                    colour = (float) heightMap[x+1][z]/ (float) maxTerrainHeightValue;
                    drawingWireframe ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                    glNormal3fv(vertexNormals[x+1][z]);
                    glVertex3f(x+1-terrainOffset, heightMap[x+1][z], z-terrainOffset);
                glEnd();
                
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                drawingWireframe = false;
                if (wireframeMode != BOTH)
                    break;
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
 * Toggles through the 3 wireframe modes.
 *****************************************/
void Terrain::changeTerrainAlgorithm(TerrainAlgorithm algorithm) {
    terrainAlgorithm = algorithm;
    generateTerrain();
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
