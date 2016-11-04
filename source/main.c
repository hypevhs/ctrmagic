#include <3ds.h>
#include <citro3d.h>
#include <stdio.h>
#include <string.h>
#include "vshader_shbin.h"
#include "myfs.h"
#include "music.h"

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

#define LANDSCAPE_TILE_SIZE 10 //WxH
#define LANDSCAPE_TILE_COUNT (LANDSCAPE_TILE_SIZE*LANDSCAPE_TILE_SIZE)
#define LANDSCAPE_TRIANGLE_COUNT (LANDSCAPE_TILE_COUNT*2)
#define LANDSCAPE_VERTEX_COUNT (LANDSCAPE_TRIANGLE_COUNT*3)
#define LANDSCAPE_VBO_SIZE (LANDSCAPE_VERTEX_COUNT*sizeof(vertex))

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
static C3D_Mtx projection;
static C3D_Mtx material =
{
	{
	{ { 0.0f, 0.2f, 0.2f, 0.2f } }, // Ambient
	{ { 0.0f, 0.4f, 0.4f, 0.4f } }, // Diffuse
	{ { 0.0f, 0.8f, 0.8f, 0.8f } }, // Specular
	{ { 1.0f, 0.0f, 0.0f, 0.0f } }, // Emission
	}
};

static vertex* vboTerrain;
static vertex* vboOrigin;
static C3D_Tex kitten_tex;
static float angleX = 0.0, angleY = 0.0, angleZ = 0.0;

static void terrainGen() {
	int n = (LANDSCAPE_TILE_SIZE + 1); //heightMapSize
	int hmSize = n * n;
	float heightMap[hmSize];
	memset(heightMap, 0, hmSize * sizeof(float));

	//todo: generate heightmap
	for (int hmidx = 0; hmidx < hmSize; hmidx++) {
		heightMap[hmidx] = rand() / (float)RAND_MAX;
	}

	vboTerrain = linearAlloc(LANDSCAPE_VBO_SIZE);
	int listIdx = 0;
	for (int x = 0; x < LANDSCAPE_TILE_SIZE; x++)
	{
		for (int y = 0; y < LANDSCAPE_TILE_SIZE; y++)
		{
			float realX0 = x / 2.0;
			float realX1 = (x + 1) / 2.0;
			float realY0 = y / 2.0;
			float realY1 = (y + 1) / 2.0;
			float mapTL = heightMap[y * n + x];
			float mapBL = heightMap[(y + 1) * n + x];
			float mapTR = heightMap[y * n + (x + 1)];
			float mapBR = heightMap[(y + 1) * n + (x + 1)];
			vertex topLef = (vertex){{realX0, mapTL, realY0}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
			vertex botLef = (vertex){{realX0, mapBL, realY1}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};
			vertex topRit = (vertex){{realX1, mapTR, realY0}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
			vertex botRit = (vertex){{realX1, mapBR, realY1}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};

			//u = p2 - p1 (botLef - topLef)
			float uA[3] = {
				botLef.position[0] - topLef.position[0],
				botLef.position[1] - topLef.position[1],
				botLef.position[2] - topLef.position[2]
			};
			//v = p3 - p1 (topRit - topLef)
			float vA[3] = {
				topRit.position[0] - topLef.position[0],
				topRit.position[1] - topLef.position[1],
				topRit.position[2] - topLef.position[2]
			};
			float normA[3] = {
				uA[1]*vA[2] - uA[2]*vA[1],
				uA[2]*vA[0] - uA[0]*vA[2],
				uA[0]*vA[1] - uA[1]*vA[0]
			};
			memcpy(topLef.normal, normA, 3*sizeof(float));
			memcpy(botLef.normal, normA, 3*sizeof(float));
			memcpy(topRit.normal, normA, 3*sizeof(float));
			vboTerrain[listIdx++] = topLef;
			vboTerrain[listIdx++] = botLef;
			vboTerrain[listIdx++] = topRit;

			//u = p2 - p1 (botLef - topRit)
			float uB[3] = {
				botLef.position[0] - topRit.position[0],
				botLef.position[1] - topRit.position[1],
				botLef.position[2] - topRit.position[2]
			};
			//v = p3 - p1 (botRit - topRit)
			float vB[3] = {
				botRit.position[0] - topRit.position[0],
				botRit.position[1] - topRit.position[1],
				botRit.position[2] - topRit.position[2]
			};
			float normB[3] = {
				uB[1]*vB[2] - uB[2]*vB[1],
				uB[2]*vB[0] - uB[0]*vB[2],
				uB[0]*vB[1] - uB[1]*vB[0]
			};
			memcpy(topLef.normal, normB, 3*sizeof(float));
			memcpy(botLef.normal, normB, 3*sizeof(float));
			memcpy(topRit.normal, normB, 3*sizeof(float));
			vboTerrain[listIdx++] = topRit;
			vboTerrain[listIdx++] = botLef;
			vboTerrain[listIdx++] = botRit;
		}
	}

	printf("listIdx is %d and it should be %d\n", listIdx, LANDSCAPE_VERTEX_COUNT);
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
	vboOrigin = linearAlloc(cube_vertex_list_count * sizeof(vertex));
	memcpy(vboOrigin, cube_vertex_list, cube_vertex_list_count * sizeof(vertex));
	printf("made vboOrigin with %d bytes\n", cube_vertex_list_count * sizeof(vertex));
	terrainGen();

	// Load the texture from file
	Handle fsHandle;
	u32 fsSize;
	fsopen(&fsHandle, &fsSize, "/3ds/ctrmagic/kitten.bin");
	char* buf = linearAlloc(sizeof(char) * fsSize);
	fsread(fsHandle, fsSize, buf);

	// and bind it to the first texture unit
	C3D_TexInit(&kitten_tex, 64, 64, GPU_RGBA8);
	C3D_TexUpload(&kitten_tex, buf);
	C3D_TexSetFilter(&kitten_tex, GPU_LINEAR , GPU_LINEAR);
	C3D_TexBind(0, &kitten_tex);

	// free the texture file
	linearFree(buf);

	// Configure the first fragment shading substage to blend the texture color with
	// the vertex color (calculated by the vertex shader using a lighting algorithm)
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
}

static void sceneRender(int eye)
{
	float iod = osGet3DSliderState();
	iod *= eye == 0 ? -0.2f : 0.2f;
	// Compute the projection matrix
	Mtx_Identity(&projection);
	Mtx_PerspStereoTilt(&projection, 80.0f*M_PI/180.0f, C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 2.0f, false);

	//for global camera
	Mtx_RotateX(&projection, M_PI / 4, true);
	Mtx_Translate(&projection, 0, -2, -2, true);

	// Calculate the modelView matrix
	C3D_Mtx modelView;
	Mtx_Identity(&modelView);
	Mtx_RotateX(&modelView, angleX, true);
	Mtx_RotateY(&modelView, angleY, true);
	Mtx_RotateZ(&modelView, angleZ, true);

	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection,   &projection);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,    &modelView);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,     &material);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, -1.0f, 0.0f, 1337.0f);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, -1.0f, 0.0f, 1337.0f);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f, 1.0f, 1337.0f);

	C3D_BufInfo bufInfo;

	//draw terrain
	BufInfo_Init(&bufInfo);
	BufInfo_Add(&bufInfo, vboTerrain, sizeof(vertex), 3, 0x210);
	C3D_SetBufInfo(&bufInfo);
	C3D_DrawArrays(GPU_TRIANGLES, 0, LANDSCAPE_VERTEX_COUNT);

	//update modelview
	Mtx_Identity(&modelView);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,    &modelView);
	//draw origin cube
	BufInfo_Init(&bufInfo);
	BufInfo_Add(&bufInfo, vboOrigin, sizeof(vertex), 3, 0x210);
	C3D_SetBufInfo(&bufInfo);
	C3D_DrawArrays(GPU_TRIANGLES, 0, cube_vertex_list_count);
}

static void sceneExit(void)
{
	// Free the texture
	C3D_TexDelete(&kitten_tex);

	// Free the VBO
	linearFree(vboTerrain);
	linearFree(vboOrigin);

	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
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

		circlePosition analog;
		hidCircleRead(&analog);
		if (analog.dx < 20 && analog.dx > -20) analog.dx = 0; //deadzones
		if (analog.dy < 20 && analog.dy > -20) analog.dy = 0;
		float howFarX = analog.dx / 160.0;
		float howFarY = analog.dy / 160.0; //no idea why its max and min is this
		float radsPerFrame = M_TAU / 90.0;

		if (kHeld & KEY_Y)
		{
			angleX += howFarX * radsPerFrame;
			angleY += howFarY * radsPerFrame;
		}
		else
		{
			angleZ += howFarX * radsPerFrame;
			angleY += howFarY * radsPerFrame;
		}

		// Render the scene twice
		C3D_RenderBufBind(&rbLeft);
		sceneRender(0);
		C3D_Flush();
		u32* leftFrame = (u32*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		C3D_RenderBufTransfer(&rbLeft, leftFrame, DISPLAY_TRANSFER_FLAGS);
		C3D_RenderBufClear(&rbLeft);

		C3D_RenderBufBind(&rbRight);
		sceneRender(1);
		C3D_Flush();
		u32* rightFrame = (u32*)gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
		C3D_RenderBufTransfer(&rbRight, rightFrame, DISPLAY_TRANSFER_FLAGS);
		C3D_RenderBufClear(&rbRight);

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
