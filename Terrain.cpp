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

        }
        displacement += 0.01;//((double) rand() / RAND_MAX);
    }
    calculateVertexNormals();
    calculateFaceNormals();
}

/*****************************************
 * Draws the generated terrain
 ****************************************/
void Terrain::drawTerrain() {

    //scale view
    glScalef(scaleFactor,scaleFactor,scaleFactor);
    
    //set materials (for lighting)
    float diffuse[4] = {.8,0,0, 1.0};
    float ambient[4] = {.8,0,0, 1.0};
    float specular[4] = {0.1,0.1,0.1, 0.5};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2);

    
    //iterate over all values in heightmap
    for (int x = 0; x < terrainSize-1; x++) {
        for (int z = 0; z < terrainSize-1; z++) {
            
            //colour is more white w/ more height, green for wireframe
            float colour = (float) heightMap[x][z]/ (float) MAX_HEIGHT;;
            bool greenColour = false;
            
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

                glNormal3fv(faceNormals[x][z]);
                glBegin(GL_QUADS);
                    glNormal3fv(vertexNormals[x][z]);
                    glVertex3f(x, heightMap[x][z], z);
                
                    glNormal3fv(vertexNormals[x+1][z]);
                    glVertex3f(x+1, heightMap[x+1][z], z);
                
                    glNormal3fv(vertexNormals[x+1][z+1]);
                    glVertex3f(x+1, heightMap[x+1][z+1], z+1);
                
                    glNormal3fv(vertexNormals[x][z+1]);
                    glVertex3f(x, heightMap[x][z+1], z+1);
                glEnd();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            
            //draw the polygons
            glNormal3fv(faceNormals[x][z]);
            
			float diffuse[4] = { .7, 0, 0.2, 1.0 };
			float grassdiff1[4] = { 0, 0.3, 0.2, 1 };
			float snowdiff[4] = { 1, 1, 1, 1 };
			float waterdiff2[4] = { 0, 0, 0.5, 1.0 };
			float ambient[4] = { .7, 0, 0.2, 1.0 };
			float grassambi1[4] = { 0, 1, 0.5, 1 };
			float snowambi[4] = { 1, 1, 1, 1 };
			float waterambi2[4] = { 0, 0, 0.7, 1 };
			float specular[4] = { 0.1, 0.1, 0.1, 0.5 };
			if (heightMap[x][z] >= 25){
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, snowambi);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, snowdiff);
			}
			if (heightMap[x][z] < 25 && heightMap[x][z] > 6){
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
			}
			if (heightMap[x][z] <= 6 && heightMap[x][z] > 5){
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grassambi1);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grassdiff1);
			}
			if (heightMap[x][z] <= 5){
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, waterambi2);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, waterdiff2);
			}
			//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (heightMap[x][z] <= 6) ? ambient3 : (heightMap[x][z] <= 4) ? ambient1 : (heightMap[x][z] >= 21) ? ambient2 : ambient);
			//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (heightMap[x][z] <= 6) ? diffuse3 : (heightMap[x][z] <= 4) ? diffuse1 : (heightMap[x][z] >= 21) ? diffuse2 : diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2);


            glBegin(GL_QUADS);
                colour = (float) heightMap[x][z]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glNormal3fv(vertexNormals[x][z]);
                glVertex3f(x, heightMap[x][z], z);
            
                colour = (float) heightMap[x][z+1]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glNormal3fv(vertexNormals[x][z+1]);
                glVertex3f(x, heightMap[x][z+1], z+1);
                
                colour = (float) heightMap[x+1][z+1]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glNormal3fv(vertexNormals[x+1][z+1]);
                glVertex3f(x+1, heightMap[x+1][z+1], z+1);
         
                colour = (float) heightMap[x+1][z]/ (float) MAX_HEIGHT;
                greenColour ? glColor3f(0, 0.8, 0.1) : glColor3f(colour, colour, colour);
                glNormal3fv(vertexNormals[x+1][z]);
                glVertex3f(x+1, heightMap[x+1][z], z);
            glEnd();

        }
    }
}

void Terrain::calculateVertexNormals() {
    
    //calculate normals
    for (int x = 0; x < terrainSize; x++) {
        for (int z = 0; z < terrainSize; z++) {
            
            //x, z
            float t1[3];
            t1[0] = x; t1[1] = heightMap[x][z]; t1[2] = z;
            
            //z + 1, x
            float t3[3];
            t3[0] = x+1; t3[1] = heightMap[x+1][z]; t3[2] = z;
            
            //x + 1, z
            float t2[3];
            t2[0] = x+1; t2[1] = heightMap[x][z+1]; t2[2] = z+1;
            
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

void Terrain::calculateFaceNormals() {

    //calculate normals
    for (int x = 0; x < terrainSize-1; x++) {
        for (int z = 0; z < terrainSize-1; z++) {
            float v1[3];
            v1[0] = vertexNormals[x][z][0];
            v1[1] = vertexNormals[x][z][1];
            v1[2] = vertexNormals[x][z][2];
            
            float v2[3];
            v2[0] = vertexNormals[x+1][z][0];
            v2[1] = vertexNormals[x+1][z][1];
            v2[2] = vertexNormals[x+1][z][2];

            
            float v3[3];
            v3[0] = vertexNormals[x+1][z+1][0];
            v3[1] = vertexNormals[x+1][z+1][1];
            v3[2] = vertexNormals[x+1][z+1][2];

            
            float v4[3];
            v4[0] = vertexNormals[x][z+1][0];
            v4[1] = vertexNormals[x][z+1][1];
            v4[2] = vertexNormals[x][z+1][2];
            
            float v[3];
            v[0] = (v1[0]+v2[0]+v3[0]+v4[0])/4.0;
            v[1] = (v1[1]+v2[1]+v3[1]+v4[1])/4.0;
            v[2] = (v1[2]+v2[2]+v3[2]+v4[2])/4.0;
            
            faceNormals[x][z][0] = v[0];
            faceNormals[x][z][1] = v[1];
            faceNormals[x][z][2] = v[2];
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
