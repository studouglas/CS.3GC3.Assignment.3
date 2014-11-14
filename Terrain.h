#define MAX_TERRAIN_SIZE 300

class Terrain {
    
    public:
    
        /****************************************
        *    CUSTOM STRUCTS
        ****************************************/
        typedef enum {SOLID, WIREFRAME, BOTH} WireframeMode;
    
        /***************************************
        *    FUNCTION DECLARATIONS
        ****************************************/
        Terrain(int size); //constructor
        
        void drawTerrain();
        void generateTerrain();
        void changeWireframeMode();
        void calculateVertexNormals();
        void calculateFaceNormals();
        char* getWireframeMode();
    
        /***************************************
        *    PUBLIC GLOBAL VARIABLES
        ****************************************/
        WireframeMode wireframeMode = SOLID;
        float heightMap[MAX_TERRAIN_SIZE][MAX_TERRAIN_SIZE];
        int terrainSize = 100;
};