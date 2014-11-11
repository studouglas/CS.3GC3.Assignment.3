#define MAX_TERRAIN_SIZE 300

class Terrain {
    
    public:
    
        /****************************************
        *    CUSTOM STRUCTS
        ****************************************/
        typedef enum {SOLID, WIREFRAME, BOTH} WireframeMode;
    
        /****************************************
         *    FUNCTION DECLARATIONS
         ****************************************/
        Terrain(int size); //constructor
        
        void drawTerrain();
        void generateTerrain();
        void changeWireframeMode();
        char* getWireframeMode();
        /****************************************
         *    PUBLIC GLOBAL VARIABLES
         ****************************************/
        float heightMap[MAX_TERRAIN_SIZE][MAX_TERRAIN_SIZE];
        float scaleFactor = 1.0;
        int terrainSize = 50;
        WireframeMode wireframeMode = SOLID;
};