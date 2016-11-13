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

#define CLEAR_COLOR 0x68B0D8FF

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
#define LANDSCAPE_SCALE_HORIZ 2

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
static C3D_Mtx projection;
static C3D_Mtx material =
{
    { //order is ? B G R
    { { 0.0f, 0.3f, 0.3f, 0.3f } }, // Ambient
    { { 0.0f, 0.7f, 0.7f, 0.7f } }, // Diffuse
    { { 0.0f, 0.1f, 0.1f, 0.1f } }, // Specular
    { { 1.0f, 0.0f, 0.0f, 0.0f } }, // Emission
    }
};

static vertex* vboTerrain;
static u16* vboTerrainIndex;
static vertex* vboOrigin;
static vertex* vboCorner;
static u16* vboCornerIndex;
static vertex* vboCastle;
static u32 vboCastleLength;
static C3D_Tex texKitten;
static C3D_Tex texLava;
static C3D_Tex texBrick;
static float camX = 4.0, camY = 4.0, camZ = 4.0, camRotX = M_PI / 4, camRotY = 0.0;
typedef struct { float position[3]; float rot[2]; } camPos;
static const camPos camFly[] = {
    {{2.234f,0.334f,1.654f},{-0.730f,0.906f}},
    {{1.149f,0.330f,2.425f},{-0.178f,0.936f}},
    {{1.437f,2.704f,2.444f},{1.046f,0.223f}},
    {{3.824f,2.065f,2.444f},{0.832f,-0.870f}},
    {{3.522f,0.468f,1.679f},{0.065f,-2.113f}}
};
int camFlyLength = sizeof(camFly) / sizeof(camFly[0]);
unsigned long long startTime;

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
    printf("gen with seed %d\n", randSeed);
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
            float texScale = 4;
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
            vboTerrain[vboIdx++] = (vertex){ {realX,realY,realZ},{x*texScale,-y*texScale},{norm[0],norm[1],norm[2]} };
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

static void loadCastleObj() {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    Handle fsHandle;
    u32 fsSize;
    fsopen(&fsHandle, &fsSize, "/3ds/ctrmagic/castle.obj");
    char* data = linearAlloc(sizeof(char) * fsSize);
    fsread(fsHandle, fsSize, data);

    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                                &num_materials, data, fsSize, TINYOBJ_FLAG_TRIANGULATE);
    linearFree(data);
    if (ret != TINYOBJ_SUCCESS) {
        printf("could not load model!!1\n");
        return;
    }

    printf("num_shapes    = %d\n", num_shapes);
    printf("num_materials = %d\n", num_materials);
    printf("num_vertices  = %d\n", attrib.num_vertices); //unique verts defined with "v"
    printf("num_faces     = %d\n", attrib.num_faces); //misleading; see below
    printf("num_face_num_v= %d\n", attrib.num_face_num_verts); //actual number of triangles to draw

    //if using drawArrays, num_faces is vbo length. elseif using drawElements, it's the length of your index array
    vboCastleLength = attrib.num_faces;
    vboCastle = (vertex*)linearAlloc(sizeof(vertex) * vboCastleLength);

    //for each indexer in the indexArray
    for (int ii = 0; ii < vboCastleLength; ii++) {
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
        vboCastle[ii] = (vertex){{x,y,z}, {vtx,vty}, {vnx,vny,vnz}};
    }
}

static void loadTexture(C3D_Tex* texStore, char* path, int sizeW, int sizeH) {
    Handle fsHandle;
    u32 fsSize;

    //load the texture from file
    fsopen(&fsHandle, &fsSize, path);
    char* buf = linearAlloc(sizeof(char) * fsSize);
    fsread(fsHandle, fsSize, buf);
    //load it into c3d
    C3D_TexInit(texStore, sizeW, sizeH, GPU_RGBA8);
    C3D_TexUpload(texStore, buf);
    C3D_TexSetFilter(texStore, GPU_LINEAR, GPU_LINEAR);
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
    printf("made vboOrigin with %d bytes\n", cube_vertex_list_count * sizeof(vertex));
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
    loadCastleObj();

    //load textures from files
    loadTexture(&texKitten, "/3ds/ctrmagic/grass.bin", 64, 64);
    loadTexture(&texLava, "/3ds/ctrmagic/lava512.bin", 512, 512);
    loadTexture(&texBrick, "/3ds/ctrmagic/brick.bin", 128, 64);

    // Configure the first fragment shading substage to blend the texture color with
    // the vertex color (calculated by the vertex shader using a lighting algorithm)
    // See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
    C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
}

//produce a unit vector
void normalize(float v[3]) {
    float length = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
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
    Mtx_RotateX(&camera, camRotX, true);
    Mtx_RotateY(&camera, camRotY, true);
    Mtx_Translate(&camera, -camX, -camY, -camZ, true);
    Mtx_Multiply(&projection, &projection, &camera);

    // Update the uniforms
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection,   &projection);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,     &material);
    float lightVec[3] = { 0.3,-0.6,0.3 };
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
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);
    //draw origin cube
    C3D_TexBind(0, &texBrick);
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboOrigin, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawArrays(GPU_TRIANGLES, 0, cube_vertex_list_count);

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

    // Free the VBO
    linearFree(vboTerrain);
    linearFree(vboTerrainIndex);
    linearFree(vboOrigin);
    linearFree(vboCorner);
    linearFree(vboCornerIndex);
    linearFree(vboCastle);

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

camPos lerpCam(camPos v0, camPos v1, float t) {
    float x = lerp(v0.position[0], v1.position[0], t);
    float y = lerp(v0.position[1], v1.position[1], t);
    float z = lerp(v0.position[2], v1.position[2], t);
    float rx = lerp(v0.rot[0], v1.rot[0], t);
    float ry = lerp(v0.rot[1], v1.rot[1], t);
    return (camPos){ {x,y,z}, {rx,ry} };
}

int main()
{
    // Initialize graphics
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    gfxSet3D(true);

    // Init the console
    consoleInit(GFX_BOTTOM, NULL);
    printf("What's up?\n");

    //init some other junk
    fsinit();
    musicinit();
    startTime = msTime();

    // Initialize the renderbuffer
    static C3D_RenderBuf rbLeft, rbRight;
    C3D_RenderBufInit(&rbLeft, 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderBufInit(&rbRight, 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    rbLeft.clearColor = CLEAR_COLOR;
    rbRight.clearColor = CLEAR_COLOR;
    C3D_RenderBufClear(&rbLeft);
    C3D_RenderBufClear(&rbRight);

    // Initialize the scene
    sceneInit();

    // Main loop
    while (aptMainLoop())
    {
        gfxSwapBuffersGpu(); // Swap the framebuffers BEFORE waiting for vblank!!!
        gspWaitForVBlank();  // wait for VBlank
        musicTick(); //update music
        hidScanInput();

        // Respond to user input
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

        //do camera flying
        u32 msElapsed = (u32)(msTime() - startTime);
        if (kHeld & KEY_L) {
            startTime += 9999999;
        }
        u32 indexFrom = msElapsed / 2000;
        u32 indexTo = indexFrom + 1;
        if (indexFrom > camFlyLength - 1) { indexFrom = camFlyLength - 1; }
        if (indexTo   > camFlyLength - 1) { indexTo   = camFlyLength - 1; }
        camPos camFrom = camFly[indexFrom];
        camPos camTo   = camFly[indexTo  ];
        float tVal = fmodf(msElapsed, 2000) / 2000;
        if (indexFrom != indexTo) { //when we end
            camPos lerped = lerpCam(camFrom, camTo, tVal);
            //now set the value to whatever we lerped to
            camX = lerped.position[0];
            camY = lerped.position[1];
            camZ = lerped.position[2];
            camRotX = lerped.rot[0];
            camRotY = lerped.rot[1];
        }

        circlePosition analog;
        hidCircleRead(&analog);
        if (analog.dx < 20 && analog.dx > -20) analog.dx = 0; //deadzones
        if (analog.dy < 20 && analog.dy > -20) analog.dy = 0;
        float howFarX = analog.dx / 160.0;
        float howFarY = analog.dy / 160.0; //no idea why its max and min is this
        float unitsPerFrame = 0.1;

        if (kHeld & KEY_R)
        {
            camRotX += -howFarY * 0.03;
            camRotY += howFarX * 0.03;
        }
        else if (kHeld & KEY_Y)
        {
            camX += howFarX * unitsPerFrame;
            camY += howFarY * unitsPerFrame;
        }
        else
        {
            camX += howFarX * unitsPerFrame;
            camZ += -howFarY * unitsPerFrame;
        }

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
    }

    // Deinitialize the scene
    sceneExit();

    // Deinitialize graphics
    C3D_Fini();
    gfxExit();
    musicFree();
    return 0;
}
