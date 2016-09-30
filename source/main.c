#include <3ds.h>
#include <citro3d.h>
#include <string.h>
#include "vshader_shbin.h"
#include <stdio.h>
#include <math.h>

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct { float x, y, z; } vertex;

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projection;

static vertex* vbo_data = NULL;
static int vtx_count = 3;

static void genVbo() {
	vbo_data = (vertex *) linearAlloc(sizeof(vertex) * 3);
	//ccw
	vbo_data[0].x = 0;
	vbo_data[0].y = 1;
	vbo_data[0].z = 0.5;

	vbo_data[1].x = -1;
	vbo_data[1].y = -1;
	vbo_data[1].z = 0.5;

	vbo_data[2].x = 1;
	vbo_data[2].y = -1;
	vbo_data[2].z = 0.5;

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 1, 0x0);
}

static void sceneInit(void)
{
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddFixed(attrInfo, 1); // v1=color

	// Set the fixed attribute (color) to solid white
	C3D_FixedAttribSet(1, 1.0, 0.0, 0.0, 1.0);

	// Compute the projection matrix
	Mtx_OrthoTilt(&projection, -1, 1, -1, 1, 0, 1);

	// Create the VBO (vertex buffer object)
	genVbo();

	// Configure the first fragment shading substage to just pass through the vertex color
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
}

static void sceneRender(void)
{
	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);

	// Draw the VBO
	C3D_DrawArrays(GPU_TRIANGLES, 0, vtx_count);
}

static void sceneExit(void)
{
	// Free the VBO
	linearFree(vbo_data);

	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}

int main()
{
	// Initialize graphics
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Init the console (NULL=default)
	consoleInit(GFX_BOTTOM, NULL);
	printf("What's up?\n");

	// Initialize the render target
	C3D_RenderTarget* target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
	C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Initialize the scene
	sceneInit();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		bool dirty = false;
		if (kHeld & KEY_RIGHT)
		{
			vbo_data[0].x += 0.005f;
		}
		if (kHeld & KEY_LEFT)
		{
			vbo_data[0].x -= 0.005f;
		}
		if (kDown & KEY_A)
		{
			genVbo();
		}
		if (kDown & KEY_B)
		{
		}
		if (kDown & KEY_X)
		{
			printf("linear free: %lu\nmappable free:%lu\n", linearSpaceFree(), mappableSpaceFree());
		}

		// Render the scene
		if (dirty)
		{
		}

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C3D_FrameDrawOn(target);
			sceneRender();
		C3D_FrameEnd(0);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	// Deinitialize the scene
	sceneExit();

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();
	return 0;
}
