#include <3ds.h>
#include <citro3d.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "vshader_shbin.h"
#include "myfs.h"
#include "music.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
#include <assert.h>
#include "diamondsquare.h"

#define CLEAR_COLOR_DAY 0x68B0D8FF
#define CLEAR_COLOR_NIT 0x0A0411FF

#define DISPLAY_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct { float position[3]; float texcoord[2]; float normal[3]; } vertex;

static const vertex cube_vertex_list[] =
{
    // First face (PZ)
    // First triangle
    { {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
    { {+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
    { {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
    // Second triangle
    { {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
    { {-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
    { {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },

    // Second face (MZ)
    // First triangle
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
    { {-0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
    { {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
    // Second triangle
    { {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
    { {+0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },

    // Third face (PX)
    // First triangle
    { {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
    { {+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
    { {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
    // Second triangle
    { {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
    { {+0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
    { {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },

    // Fourth face (MX)
    // First triangle
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
    { {-0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
    { {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
    // Second triangle
    { {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
    { {-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },

    // Fifth face (PY)
    // First triangle
    { {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
    { {-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
    { {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
    // Second triangle
    { {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
    { {+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
    { {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },

    // Sixth face (MY)
    // First triangle
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
    { {+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
    { {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
    // Second triangle
    { {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
    { {-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
    { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
};

#define cube_vertex_list_count (sizeof(cube_vertex_list)/sizeof(cube_vertex_list[0]))

#define LANDSCAPE_TILE_SIZE 64 //WxH
#define LANDSCAPE_INDEX_COUNT (((LANDSCAPE_TILE_SIZE+1)*LANDSCAPE_TILE_SIZE*2) + ((LANDSCAPE_TILE_SIZE-1)*2))
//2 extra verts between each row are added for degenerate tris
#define LANDSCAPE_VERTEX_COUNT ((LANDSCAPE_TILE_SIZE+1)*(LANDSCAPE_TILE_SIZE+1))
#define LANDSCAPE_SCALE_HORIZ 6
#define LANDSCAPE_TEXTURE_SCALE 2

static PrintConsole consoleMain, consoleBottom;

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
static C3D_Mtx projection;
static C3D_Mtx material =
{
    { //order is ? B G R
    { { 0.0f, 0.1f, 0.1f, 0.1f } }, // Ambient
    { { 0.0f, 0.7f, 0.7f, 0.7f } }, // Diffuse
    { { 0.0f, 0.1f, 0.1f, 0.1f } }, // Specular
    { { 1.0f, 0.0f, 0.0f, 0.0f } }, // Emission
    }
};

//generated/hardcoded vbos
static vertex* vboTerrain;
static u16* vboTerrainIndex;
static vertex* vboOrigin;
static vertex* vboCorner;
static u16* vboCornerIndex;
//obj vbos
static vertex* vboCastle;
static u32 vboCastleLength;
static vertex* vboPlr;
static u32 vboPlrLength;
static vertex* vboPlrWheel;
static u32 vboPlrWheelLength;
//textures
static C3D_Tex texKitten;
static C3D_Tex texLava;
static C3D_Tex texBrick;
static C3D_Tex texPlr;
static C3D_Tex texPlrWheel;
//state vars
static float plrX = 0, plrY = 22, plrZ = 20;
static float plrRotFacing = 0;
static float plrSpeedHoriz = 0, plrSpeedVert = 0;
static bool plrAerial = true;
static C3D_FQuat plrRot = {{0,0,0,0}};
#define PLRHACCEL 0.001
#define PLRMAXSPEED 0.3
#define PLRGRAVITY 0.004
static float camFollowDist = 3;
static float camAngle = M_PI / 8;
unsigned long long startTime;
static int frameCounter = 0;
static float dirLight = 0;

//produce a unit vector
void normalize(float v[3]) {
    float length = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
}

static void calcNormal(float norm[3], float v0[3], float v1[3], float v2[3]) {
    //u = p1 - p0
    float uA[3] = {
        v1[0] - v0[0],
        v1[1] - v0[1],
        v1[2] - v0[2]
    };
    //v = p2 - p0
    float vA[3] = {
        v2[0] - v0[0],
        v2[1] - v0[1],
        v2[2] - v0[2]
    };
    norm[0] = uA[1]*vA[2] - uA[2]*vA[1];
    norm[1] = uA[2]*vA[0] - uA[0]*vA[2];
    norm[2] = uA[0]*vA[1] - uA[1]*vA[0];
}

static void normalOnTerrain(float normOut[3], float traceX, float traceZ) {
    //which quad are you in?
    float terrainOffset = (LANDSCAPE_TILE_SIZE * LANDSCAPE_SCALE_HORIZ) / 2.0;
    float indexX = (traceX + terrainOffset) / LANDSCAPE_SCALE_HORIZ;
    float indexZ = (traceZ + terrainOffset) / LANDSCAPE_SCALE_HORIZ;
    int vboWidth = LANDSCAPE_TILE_SIZE + 1;
    //get height at each corner of the quad
    int vboTLIdx = ((int)indexX + 0) + (((int)indexZ + 0) * vboWidth);
    int vboTRIdx = ((int)indexX + 1) + (((int)indexZ + 0) * vboWidth);
    int vboBLIdx = ((int)indexX + 0) + (((int)indexZ + 1) * vboWidth);
    int vboBRIdx = ((int)indexX + 1) + (((int)indexZ + 1) * vboWidth);
    assert(vboTLIdx >= 0 && vboTLIdx < LANDSCAPE_VERTEX_COUNT);
    assert(vboTRIdx >= 0 && vboTRIdx < LANDSCAPE_VERTEX_COUNT);
    assert(vboBLIdx >= 0 && vboBLIdx < LANDSCAPE_VERTEX_COUNT);
    assert(vboBRIdx >= 0 && vboBRIdx < LANDSCAPE_VERTEX_COUNT);
    //which triangle are we on? the top or bottom half of the quad?
    float fractionalIdxX = indexX - ((long)indexX);
    float fractionalIdxZ = indexZ - ((long)indexZ);
    assert(fractionalIdxX >= 0 && fractionalIdxX <= 1);
    assert(fractionalIdxZ >= 0 && fractionalIdxZ <= 1);
    bool onTopHalf = (1 - fractionalIdxX) > fractionalIdxZ;

    float TRPos[3];
    float TLPos[3];
    float BLPos[3];
    float BRPos[3];

    memcpy(TRPos, vboTerrain[vboTRIdx].position, 3*sizeof(float));
    memcpy(TLPos, vboTerrain[vboTLIdx].position, 3*sizeof(float));
    memcpy(BLPos, vboTerrain[vboBLIdx].position, 3*sizeof(float));
    memcpy(BRPos, vboTerrain[vboBRIdx].position, 3*sizeof(float));

    //because the scale of the terrain affects the normal, but the
    //scale is a mtx trans, we apply it manually
    TRPos[0] *= LANDSCAPE_SCALE_HORIZ;
    TRPos[2] *= LANDSCAPE_SCALE_HORIZ;
    TLPos[0] *= LANDSCAPE_SCALE_HORIZ;
    TLPos[2] *= LANDSCAPE_SCALE_HORIZ;
    BLPos[0] *= LANDSCAPE_SCALE_HORIZ;
    BLPos[2] *= LANDSCAPE_SCALE_HORIZ;
    BRPos[0] *= LANDSCAPE_SCALE_HORIZ;
    BRPos[2] *= LANDSCAPE_SCALE_HORIZ;

    if (onTopHalf) {
        //topright, topleft, bottomleft
        calcNormal(normOut, TRPos, TLPos, BLPos);
    } else {
        //topright, bottomleft, bottomright
        calcNormal(normOut, TRPos, BLPos, BRPos);
    }

    normalize(normOut);
}

/*
mesh around # looks like this. use these points to calculate avg face normal
   .-.
  /|/|
 .-#-.
 |/|/
 .-.
*/
static void diamondNormal(float normOut[3], float self, float uu, float ur, float rr, float dd, float dl, float ll) {
    float vself[3] = {0,self,0};
    float vuu[3] = { 0, uu, 1};
    float vur[3] = { 1, ur, 1};
    float vrr[3] = { 1, rr, 0};
    float vdd[3] = { 0, dd,-1};
    float vdl[3] = {-1, dl,-1};
    float vll[3] = {-1, ll, 0};
    float n1[3]; calcNormal(n1, vself, vuu, vur);
    float n2[3]; calcNormal(n2, vself, vur, vrr);
    float n3[3]; calcNormal(n3, vself, vrr, vdd);
    float n4[3]; calcNormal(n4, vself, vdd, vdl);
    float n5[3]; calcNormal(n5, vself, vdl, vll);
    float n6[3]; calcNormal(n6, vself, vll, vuu);
    normOut[0] = (n1[0]+n2[0]+n3[0]+n4[0]+n5[0]+n6[0]) / 6.0;
    normOut[1] = (n1[1]+n2[1]+n3[1]+n4[1]+n5[1]+n6[1]) / 6.0;
    normOut[2] = (n1[2]+n2[2]+n3[2]+n4[2]+n5[2]+n6[2]) / 6.0;
}

static int randSeed = 54; //known seed

static void terrainGen() {
    srand(randSeed);
    printf("diamndsqr seed = %d\n", randSeed);
    int n = (LANDSCAPE_TILE_SIZE + 1); //heightMapSize
    int hmSize = n * n;
    double* heightMap = linearAlloc(hmSize * sizeof(double));

    //generate heightmap
    diamondSquare(heightMap, n, n);

    //smooth out where castle is
    int hX = n / 2;
    int hY = n / 2;
    float setTo = heightMap[hY * n + hX];
    heightMap[(hY-1) * n + (hX-1)] = setTo;
    heightMap[(hY-1) * n + (hX+0)] = setTo;
    heightMap[(hY-1) * n + (hX+1)] = setTo;
    heightMap[(hY+0) * n + (hX-1)] = setTo;
    heightMap[(hY+0) * n + (hX+0)] = setTo;
    heightMap[(hY+0) * n + (hX+1)] = setTo;
    heightMap[(hY+1) * n + (hX-1)] = setTo;
    heightMap[(hY+1) * n + (hX+0)] = setTo;
    heightMap[(hY+1) * n + (hX+1)] = setTo;

    vboTerrain = linearAlloc(LANDSCAPE_VERTEX_COUNT * sizeof(vertex));
    vboTerrainIndex = linearAlloc(LANDSCAPE_INDEX_COUNT * sizeof(u16));

    //for the vbo, we just make a vert for every heightmap pt
    int vboIdx = 0;
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            float realX = x;
            float realY = heightMap[y * n + x];
            float realZ = y;
            float norm[] = {0, 1, 0}; //up by default
            //if we have neighboring points, calculate the normal from the "diamond"
            if (y >= 1 && y < n - 1 && x >= 1 && x < n - 1) {
                float self = realY;
                float uu = heightMap[(y + 1) * n + (x + 0)];
                float ur = heightMap[(y + 1) * n + (x + 1)];
                float rr = heightMap[(y + 0) * n + (x + 1)];
                float dd = heightMap[(y - 1) * n + (x + 0)];
                float dl = heightMap[(y - 1) * n + (x - 1)];
                float ll = heightMap[(y + 0) * n + (x - 1)];
                diamondNormal(norm, self, uu, ur, rr, dd, dl, ll);
            }
            vboTerrain[vboIdx++] = (vertex){ {realX,realY,realZ},{x*LANDSCAPE_TEXTURE_SCALE,-y*LANDSCAPE_TEXTURE_SCALE},{norm[0],norm[1],norm[2]} };
        }
    }
    assert(vboIdx == LANDSCAPE_VERTEX_COUNT);

    //then the indexer which is a bunch harder
    int idxIdx = 0;
    //for each row except for the last one
    for (int y = 0; y < n - 1; y++) {
        //for every column including the last one
        for (int x = 0; x < n; x++) {
            int idxHere = y * n + x; //for here
            int idxBelo = (y+1) * n + x; //for the one underneath
            //add the topLeft then the bottomLeft
            vboTerrainIndex[idxIdx++] = idxHere;
            vboTerrainIndex[idxIdx++] = idxBelo;
        }
        //with every row-end except for the last row-end,
        if (y != n-2) {
            //add 2 verts to make some degenerate tris to reset the primitive
            //first, the last in the bottom row
            int idxA = (y+1) * n + (n-1);
            vboTerrainIndex[idxIdx++] = idxA;
            //then, the first in bottom row
            int idxB = (y+1) * n + (0);
            vboTerrainIndex[idxIdx++] = idxB;
        }
    }
    assert(idxIdx == LANDSCAPE_INDEX_COUNT);
    linearFree(heightMap);
}

//allocates and stores obj model into **vbo and stores the length in *u32
static void loadObj(char* filename, vertex** vbo, u32* length) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    u32 fsSize;
    FILE* fsHandle = fsopen(filename, &fsSize);
    char* data = linearAlloc(sizeof(char) * fsSize);
    fsread(fsHandle, fsSize, data);

    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                                &num_materials, data, fsSize, TINYOBJ_FLAG_TRIANGULATE);
    linearFree(data);
    if (ret != TINYOBJ_SUCCESS) {
        printf("could not load model!!1\n");
        return;
    }

    //attrib.num_vertices is unique verts defined with "v"
    //attrib.num_faces is misleading; see below
    //attrib.num_face_num_verts is actual number of triangles to draw

    //if using drawArrays, num_faces is vbo length. elseif using drawElements, it's the length of your index array
    *length = attrib.num_faces;
    *vbo = (vertex*)linearAlloc(sizeof(vertex) * (*length));

    //for each indexer in the indexArray
    for (int ii = 0; ii < *length; ii++) {
        tinyobj_vertex_index_t v0 = attrib.faces[ii];
        //get the vertex pos/norm/etc of wherever that indexer points to
        float x = attrib.vertices[v0.v_idx * 3 + 0];
        float y = attrib.vertices[v0.v_idx * 3 + 1];
        float z = attrib.vertices[v0.v_idx * 3 + 2];
        float vtx = attrib.texcoords[v0.vt_idx * 2 + 0];
        float vty = attrib.texcoords[v0.vt_idx * 2 + 1];
        float vnx = attrib.normals[v0.vn_idx * 3 + 0];
        float vny = attrib.normals[v0.vn_idx * 3 + 1];
        float vnz = attrib.normals[v0.vn_idx * 3 + 2];
        //and add it to vbo
        (*vbo)[ii] = (vertex){{x,y,z}, {vtx,vty}, {vnx,vny,vnz}};
    }
}

static void loadTexture(C3D_Tex* texStore, char* path, int sizeW, int sizeH) {
    FILE* fsHandle;
    u32 fsSize;

    //load the texture from file
    fsHandle = fsopen(path, &fsSize);
    char* buf = linearAlloc(sizeof(char) * fsSize);
    fsread(fsHandle, fsSize, buf);
    //load it into c3d
    C3D_TexInit(texStore, sizeW, sizeH, GPU_RGBA8);
    C3D_TexUpload(texStore, buf);
    C3D_TexSetFilter(texStore, GPU_LINEAR, GPU_NEAREST);
    C3D_TexSetWrap(texStore, GPU_REPEAT, GPU_REPEAT);
    //free resources
    linearFree(buf);
}

static void sceneInit(void)
{
    // Load the vertex shader, create a shader program and bind it
    vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    // Get the location of the uniforms
    uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
    uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");
    uLoc_lightVec     = shaderInstanceGetUniformLocation(program.vertexShader, "lightVec");
    uLoc_lightHalfVec = shaderInstanceGetUniformLocation(program.vertexShader, "lightHalfVec");
    uLoc_lightClr     = shaderInstanceGetUniformLocation(program.vertexShader, "lightClr");
    uLoc_material     = shaderInstanceGetUniformLocation(program.vertexShader, "material");

    // Configure attributes for use with the vertex shader
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
    AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal

    //create the VBOs
    //origin
    vboOrigin = linearAlloc(cube_vertex_list_count * sizeof(vertex));
    memcpy(vboOrigin, cube_vertex_list, cube_vertex_list_count * sizeof(vertex));
    //terrain
    terrainGen();
    //corner and index
    vboCorner = linearAlloc(6 * sizeof(vertex));
    vboCorner[0] = (vertex){ {-.5, 1, .5},{0,0},{-M_SQRT2,0, M_SQRT2} };
    vboCorner[1] = (vertex){ {-.5, 0, .5},{0,1},{-M_SQRT2,0, M_SQRT2} };
    vboCorner[2] = (vertex){ { .5, 1, .5},{1,0},{ M_SQRT2,0, M_SQRT2} };
    vboCorner[3] = (vertex){ { .5, 0, .5},{1,1},{ M_SQRT2,0, M_SQRT2} };
    vboCorner[4] = (vertex){ { .5, 1,-.5},{0,0},{ M_SQRT2,0,-M_SQRT2} };
    vboCorner[5] = (vertex){ { .5, 0,-.5},{0,1},{ M_SQRT2,0,-M_SQRT2} };
    vboCornerIndex = linearAlloc(12 * sizeof(u16));
    u16 idxArray[] = { 0,1,2,3,4,5 };
    memcpy(vboCornerIndex, idxArray, 6*sizeof(u16));
    //castle
    loadObj("/3ds/ctrmagic/castle.obj", &vboCastle, &vboCastleLength);
    //player
    loadObj("/3ds/ctrmagic/ybody.obj", &vboPlr, &vboPlrLength);
    loadObj("/3ds/ctrmagic/ywheels.obj", &vboPlrWheel, &vboPlrWheelLength);

    //load textures from files
    loadTexture(&texKitten, "/3ds/ctrmagic/grass.bin", 64, 64);
    loadTexture(&texLava, "/3ds/ctrmagic/lava512.bin", 512, 512);
    loadTexture(&texBrick, "/3ds/ctrmagic/brick.bin", 128, 64);
    loadTexture(&texPlr, "/3ds/ctrmagic/ybody.bin", 256, 256);
    loadTexture(&texPlrWheel, "/3ds/ctrmagic/ywheels.bin", 128, 128);

    // Configure the first fragment shading substage to blend the texture color with
    // the vertex color (calculated by the vertex shader using a lighting algorithm)
    // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
    C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
    printf("done initializing scene\n");
}

static void sceneRender(int eye)
{
    float iod = osGet3DSliderState();
    iod *= eye == 0 ? -0.05f : 0.05f;
    // Compute the projection matrix
    Mtx_Identity(&projection);
    Mtx_PerspStereoTilt(&projection, 50.0f*M_PI/180.0f, C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 10.0, false);

    //for global camera
    C3D_Mtx camera;
    Mtx_Identity(&camera);
    Mtx_Translate(&camera, 0, 0, -camFollowDist, true); //follow dist
    Mtx_RotateX(&camera, camAngle, true); //follow angle
    Mtx_RotateY(&camera, plrRotFacing, true);
    Mtx_Translate(&camera, -plrX, -plrY, -plrZ, true);
    Mtx_Multiply(&projection, &projection, &camera);

    //no diffuse when nighttime
    float amt = (sinf(dirLight) * 3);
    if (amt > 1) { amt = 1; }
    if (amt <-1) { amt =-1; }
    amt += 1;
    amt /= 2;
    amt = 1 - amt;
    material.r[1].z = 0.7 * amt;
    material.r[1].y = 0.7 * amt;
    material.r[1].x = 0.7 * amt;

    // Update the uniforms
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection,   &projection);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,     &material);
    float lightVec[3] = { cosf(dirLight),sinf(dirLight), -0.5 };
    normalize(lightVec);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     lightVec[0], lightVec[1], lightVec[2], 1337.0f);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, lightVec[0], lightVec[1], lightVec[2], 1337.0f);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f, 1.0f, 1337.0f);

    C3D_BufInfo bufInfo;
    C3D_Mtx modelView;

    //update modelview
    Mtx_Identity(&modelView);
    float offset = (LANDSCAPE_TILE_SIZE * LANDSCAPE_SCALE_HORIZ) / 2.0;
    Mtx_Translate(&modelView, -offset, 0, -offset, true);
    Mtx_Scale(&modelView, LANDSCAPE_SCALE_HORIZ, 1, LANDSCAPE_SCALE_HORIZ);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);
    //draw terrain
    C3D_TexBind(0, &texKitten);
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboTerrain, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawElements(GPU_TRIANGLE_STRIP, LANDSCAPE_INDEX_COUNT, 1, vboTerrainIndex);

    //update modelview
    Mtx_Identity(&modelView);
    Mtx_Scale(&modelView, 0.07f, 0.07f, 0.07f);
    C3D_Mtx rotateMtx;
    Mtx_FromQuat(&rotateMtx, plrRot);
    Mtx_Multiply(&modelView, &rotateMtx, &modelView);
    Mtx_Translate(&modelView, plrX, plrY, plrZ, false);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);
    //draw player
    C3D_TexBind(0, &texPlr);
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboPlr, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawArrays(GPU_TRIANGLES, 0, vboPlrLength);
    //and wheels, with the same modelview
    C3D_TexBind(0, &texPlrWheel);
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboPlrWheel, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawArrays(GPU_TRIANGLES, 0, vboPlrWheelLength);

    //update modelview
    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, 0, 1, 0, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);
    //draw corner vbo
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboCorner, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawElements(GPU_TRIANGLE_STRIP, 6, 1, vboCornerIndex);

    //update modelview
    Mtx_Identity(&modelView);
    Mtx_Translate(&modelView, 0, vboTerrain[LANDSCAPE_VERTEX_COUNT / 2].position[1], 0, true);
    Mtx_Translate(&modelView, 0, -0.3, 0, true);
    Mtx_Scale(&modelView, 4,4,4);
    Mtx_RotateY(&modelView, -M_PI_2, false);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);
    //draw castle
    C3D_TexBind(0, &texBrick);
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboCastle, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawArrays(GPU_TRIANGLES, 0, vboCastleLength);
}

static void sceneExit(void)
{
    // Free the texture
    C3D_TexDelete(&texKitten);
    C3D_TexDelete(&texLava);
    C3D_TexDelete(&texBrick);
    C3D_TexDelete(&texPlr);
    C3D_TexDelete(&texPlrWheel);

    // Free the VBO
    linearFree(vboTerrain);
    linearFree(vboTerrainIndex);
    linearFree(vboOrigin);
    linearFree(vboCorner);
    linearFree(vboCornerIndex);
    linearFree(vboCastle);
    linearFree(vboPlr);
    linearFree(vboPlrWheel);

    // Free the shader program
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
}

unsigned long long msTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long msTime =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
    return msTime;
}

float lerp(float v0, float v1, float t) {
    return (1-t)*v0 + t*v1;
}

static inline float dot2d(float lhs[2], float rhs[2])
{
    return lhs[0]*rhs[0] + lhs[1]*rhs[1];
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
static void Barycentric(float p[2], float a[2], float b[2], float c[2], float *u, float *v, float *w)
{
    float v0[2] = { b[0] - a[0], b[1] - a[1] };
    float v1[2] = { c[0] - a[0], c[1] - a[1] };
    float v2[2] = { p[0] - a[0], p[1] - a[1] };
    float d00 = dot2d(v0, v0);
    float d01 = dot2d(v0, v1);
    float d11 = dot2d(v1, v1);
    float d20 = dot2d(v2, v0);
    float d21 = dot2d(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    *v = (d11 * d20 - d01 * d21) / denom;
    *w = (d00 * d21 - d01 * d20) / denom;
    *u = 1.0f - *v - *w;
}

static float pointOnTerrain(float traceX, float traceZ) {
    //which quad are you in?
    float terrainOffset = (LANDSCAPE_TILE_SIZE * LANDSCAPE_SCALE_HORIZ) / 2.0;
    float indexX = (traceX + terrainOffset) / LANDSCAPE_SCALE_HORIZ;
    float indexZ = (traceZ + terrainOffset) / LANDSCAPE_SCALE_HORIZ;
    int vboWidth = LANDSCAPE_TILE_SIZE + 1;
    //get height at each corner of the quad
    int vboTLIdx = ((int)indexX + 0) + (((int)indexZ + 0) * vboWidth);
    int vboTRIdx = ((int)indexX + 1) + (((int)indexZ + 0) * vboWidth);
    int vboBLIdx = ((int)indexX + 0) + (((int)indexZ + 1) * vboWidth);
    int vboBRIdx = ((int)indexX + 1) + (((int)indexZ + 1) * vboWidth);
    assert(vboTLIdx >= 0 && vboTLIdx < LANDSCAPE_VERTEX_COUNT);
    assert(vboTRIdx >= 0 && vboTRIdx < LANDSCAPE_VERTEX_COUNT);
    assert(vboBLIdx >= 0 && vboBLIdx < LANDSCAPE_VERTEX_COUNT);
    assert(vboBRIdx >= 0 && vboBRIdx < LANDSCAPE_VERTEX_COUNT);
    float vboTLY = vboTerrain[vboTLIdx].position[1];
    float vboTRY = vboTerrain[vboTRIdx].position[1];
    float vboBLY = vboTerrain[vboBLIdx].position[1];
    float vboBRY = vboTerrain[vboBRIdx].position[1];
    //which triangle are we on? the top or bottom half of the quad?
    float fractionalIdxX = indexX - ((long)indexX);
    float fractionalIdxZ = indexZ - ((long)indexZ);
    assert(fractionalIdxX >= 0 && fractionalIdxX <= 1);
    assert(fractionalIdxZ >= 0 && fractionalIdxZ <= 1);
    bool onTopHalf = (1 - fractionalIdxX) > fractionalIdxZ;
    //get barycentric coordinates for the triangle
    float bU, bV, bW;
    float bP[2] = { fractionalIdxX, fractionalIdxZ };
    // c_b
    // |/|
    // a c
    float bA[2] = { 0, 1 }; //BL
    float bB[2] = { 1, 0 }; //TR
    float bC[2];
    if (onTopHalf) {
        bC[0] = 0; bC[1] = 0; //TL
    } else {
        bC[0] = 1; bC[1] = 1; //BR
    }
    Barycentric(bP, bA, bB, bC, &bU, &bV, &bW);
    //use these coordinates as our "weights" toward each of the 3 verts
    //in this triangle. that's the Y-value for the point that lies
    //within the tri.
    if (onTopHalf) {
        return bU*vboBLY + bV*vboTRY + bW*vboTLY;
    } else {
        return bU*vboBLY + bV*vboTRY + bW*vboBRY;
    }
}

void updateScene() {
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();
    circlePosition analog;
    hidCircleRead(&analog);
    if (analog.dx < 20 && analog.dx > -20) analog.dx = 0; //deadzones
    if (analog.dy < 20 && analog.dy > -20) analog.dy = 0;
    float howFarX = analog.dx / 160.0;
    //float howFarY = analog.dy / 160.0; //no idea why its max and min is this

    if (kHeld & KEY_R) {
        if (kHeld & KEY_DUP)
            camFollowDist -= 0.07;
        if (kHeld & KEY_DDOWN)
            camFollowDist += 0.07;
    } else {
        if (kHeld & KEY_DUP)
            camAngle += 0.02;
        if (kHeld & KEY_DDOWN)
            camAngle -= 0.02;
    }

    if (kHeld & KEY_DRIGHT) {
        dirLight += 0.01;
    }
    if (kHeld & KEY_DLEFT) {
        dirLight -= 0.01;
    }

    if (kDown & KEY_L) {
        float norm[3];
        normalOnTerrain(norm, plrX, plrZ);
        printf("Pos   : [%.3f %.3f %.3f]\n", plrX, plrY, plrZ);
        printf("Norm  : [%.3f %.3f %.3f]\n", norm[0], norm[1], norm[2]);
        printf("Aerial: [%s]\n", plrAerial ? "true" : "false");
    }

    //rotate player
    plrRotFacing += howFarX * 0.05;
    plrRotFacing = fmodf(plrRotFacing, 2*M_PI);

    //update speeds
    plrSpeedVert -= PLRGRAVITY;
    if (kHeld & KEY_A) {
        plrSpeedHoriz += PLRHACCEL;
    } else {
        plrSpeedHoriz -= PLRHACCEL;
    }
    if (kHeld & KEY_B) {
        plrSpeedHoriz *= 0.95f;
    }
    plrSpeedHoriz = fmaxf(0, fminf(PLRMAXSPEED, plrSpeedHoriz));

    //calculate next position
    float plrXNext = plrX;
    float plrYNext = plrY;
    float plrZNext = plrZ;
    plrXNext += sinf(plrRotFacing) * plrSpeedHoriz;
    plrZNext += cosf(plrRotFacing) * -plrSpeedHoriz;
    plrYNext += plrSpeedVert;

    //get terrain point there
    float terrainUnderYou = pointOnTerrain(plrXNext, plrZNext);

    //if less than terrain point, snap to terrain
    if (plrYNext - 0.01 < terrainUnderYou) {
        plrYNext = terrainUnderYou;
        plrAerial = false;
    } else {
        plrAerial = true;
    }

    //carry vertical ground movement momentum into aerial
    plrSpeedVert = plrYNext - plrY;

    //set positions
    plrX = plrXNext;
    plrY = plrYNext;
    plrZ = plrZNext;

    //update rotation quat if on ground
    if (!plrAerial) {
        float norm[3];
        normalOnTerrain(norm, plrX, plrZ);
        C3D_FVec src = FVec3_New(0, 0, 0);
        C3D_FVec nrm = FVec3_New(norm[0], norm[1], norm[2]);
        C3D_FVec fwd = FVec3_New(0,0,-1);
        C3D_FVec up = FVec3_New(0,1,0);
        C3D_FQuat look = Quat_LookAt(src, nrm, fwd, up);
        plrRot = Quat_Identity();
        plrRot = Quat_Multiply(plrRot, look);
        float ang = -plrRotFacing + FVec3_Dot(nrm, FVec3_New(1,0,0));
        plrRot = Quat_Rotate(plrRot, nrm, ang, true); //rotate about normal to face camera-forward

        //fix model: rot 90 about X, then 180 about Y
        plrRot = Quat_Rotate(plrRot, FVec3_New(1,0,0), -M_PI_2, false);
        plrRot = Quat_Rotate(plrRot, FVec3_New(0,1,0), M_PI, false);
    }
}

void updateSkyColor(C3D_RenderBuf *rbLeft, C3D_RenderBuf *rbRight) {
    float amt = (sinf(dirLight) * 3);
    if (amt > 1) { amt = 1; }
    if (amt <-1) { amt =-1; }
    amt += 1;
    amt /= 2;

    float dR = ((CLEAR_COLOR_DAY >> 24) & 0xFF) / (float)0xFF; //0.0 to 1.0
    float dG = ((CLEAR_COLOR_DAY >> 16) & 0xFF) / (float)0xFF;
    float dB = ((CLEAR_COLOR_DAY >> 8 ) & 0xFF) / (float)0xFF;

    float nR = ((CLEAR_COLOR_NIT >> 24) & 0xFF) / (float)0xFF;
    float nG = ((CLEAR_COLOR_NIT >> 16) & 0xFF) / (float)0xFF;
    float nB = ((CLEAR_COLOR_NIT >> 8 ) & 0xFF) / (float)0xFF;

    float lR = lerp(dR, nR, amt);
    float lG = lerp(dG, nG, amt);
    float lB = lerp(dB, nB, amt);

    u8 fR = (u8)(lR * 0xFF);
    u8 fG = (u8)(lG * 0xFF);
    u8 fB = (u8)(lB * 0xFF);

    u32 finalColor = (fR << 24) | (fG << 16) | (fB << 8) | 0xFF;
    rbLeft->clearColor = finalColor;
    rbRight->clearColor = finalColor;
}

int main()
{
    // Initialize graphics
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    gfxSet3D(true);

    // Init the console
    consoleInit(GFX_BOTTOM, &consoleMain);
    consoleInit(GFX_BOTTOM, &consoleBottom);
    consoleSetWindow(&consoleMain,   0,  0, 40, 28);
    consoleSetWindow(&consoleBottom, 0, 28, 40,  1);
    consoleSelect(&consoleMain);
    printf("========================================");
    printf("=             Castle Scene             =");
    printf("========================================");

    //init some other junk
    musicinit();
    startTime = msTime();

    // Initialize the renderbuffer
    static C3D_RenderBuf rbLeft, rbRight;
    C3D_RenderBufInit(&rbLeft, 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderBufInit(&rbRight, 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    rbLeft.clearColor = CLEAR_COLOR_DAY;
    rbRight.clearColor = CLEAR_COLOR_DAY;
    C3D_RenderBufClear(&rbLeft);
    C3D_RenderBufClear(&rbRight);

    // Initialize the scene
    sceneInit();

    // Main loop
    while (aptMainLoop())
    {
        gfxSwapBuffersGpu(); // Swap the framebuffers BEFORE waiting for vblank!!!
        gspWaitForVBlank();  // wait for VBlank
        struct timeval clockBegin; gettimeofday(&clockBegin, NULL);
        musicTick(); //update music
        hidScanInput();

        // Respond to user input
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

        updateScene();

        //get clear colors
        updateSkyColor(&rbLeft, &rbRight);

        // Render the scene twice
        C3D_RenderBufBind(&rbLeft);
        sceneRender(0);
        C3D_Flush();
        u32* leftFrame = (u32*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
        C3D_RenderBufTransfer(&rbLeft, leftFrame, DISPLAY_TRANSFER_FLAGS);
        C3D_RenderBufClear(&rbLeft);

        if (osGet3DSliderState() != 0) {
            C3D_RenderBufBind(&rbRight);
            sceneRender(1);
            C3D_Flush();
            u32* rightFrame = (u32*)gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
            C3D_RenderBufTransfer(&rbRight, rightFrame, DISPLAY_TRANSFER_FLAGS);
            C3D_RenderBufClear(&rbRight);
        }

        // Flush the framebuffers out of the data cache (not necessary with pure GPU rendering)
        //gfxFlushBuffers();

        struct timeval clockEnd; gettimeofday(&clockEnd, NULL);
        double clockSec = (double)(clockEnd.tv_usec - clockBegin.tv_usec) / 1000000 + (double)(clockEnd.tv_sec - clockBegin.tv_sec);
        consoleSelect(&consoleBottom);
        if (frameCounter % 30 == 0) {
            printf("\nmstime: %8.3f", clockSec*1000.0f);
        }
        consoleSelect(&consoleMain);

        frameCounter++;
    }

    // Deinitialize the scene
    sceneExit();

    // Deinitialize graphics
    C3D_Fini();
    gfxExit();
    musicFree();
    return 0;
}
