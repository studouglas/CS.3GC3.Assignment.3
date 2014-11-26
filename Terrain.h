#define MAX_TERRAIN_SIZE 300

class Terrain {
    
    public:
    
        /****************************************
        *    CUSTOM STRUCTS
        ****************************************/
        typedef enum {SOLID, WIREFRAME, BOTH} WireframeMode;
        typedef enum {FAULT, CIRCLE} TerrainAlgorithm;
    
        /***************************************
        *    FUNCTION DECLARATIONS
        ****************************************/
        Terrain(int size); //constructor
        
        void drawTerrain();
        void generateTerrain();
        void smoothTerrain(float);
        void changeWireframeMode();
        void changeTerrainAlgorithm(TerrainAlgorithm);
        void calculateVertexNormals();
        void calculateFaceNormals();
        char* getWireframeMode();
        char* getAlgorithm();
    
        /***************************************
        *    PUBLIC GLOBAL VARIABLES
        ****************************************/
        WireframeMode wireframeMode = SOLID;
        TerrainAlgorithm terrainAlgorithm = FAULT;
        float heightMap[MAX_TERRAIN_SIZE][MAX_TERRAIN_SIZE];
        int terrainSize = 100;
};