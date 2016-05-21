#include <3ds.h>
#include <citro3d.h>
#include <string.h>
#include "vshader_shbin.h"

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct { float x, y, z; } vertex;
typedef struct { int x, y, w, h; } intrect;
static int depthLevel = 0; // one square
#define sqr_count (1<<(3*depthLevel))
#define tri_count (sqr_count*2)
#define vtx_count (tri_count*3)

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projection;

static void* vbo_data;

static int rightX = 80;

void addSquare(vertex* vtx, int* arrayPos, intrect bounding) {
	float theZee = 0.5f;
	vertex topLeft, topRight, bottomLeft, bottomRight;
	topLeft.z = theZee;
	topRight.z = theZee;
	bottomLeft.z = theZee;
	bottomRight.z = theZee;
	
	topLeft.x = bounding.x;
	topLeft.y = bounding.y;
	topRight.x = bounding.x + bounding.w;
	topRight.y = bounding.y;
	bottomLeft.x = bounding.x;
	bottomLeft.y = bounding.y + bounding.h;
	bottomRight.x = bounding.x + bounding.w;
	bottomRight.y = bounding.y + bounding.h;
	
	//tri 1
	vtx[*arrayPos] = topLeft;
	(*arrayPos)++;
	vtx[*arrayPos] = bottomLeft;
	(*arrayPos)++;
	vtx[*arrayPos] = topRight;
	(*arrayPos)++;
	//tri 2
	vtx[*arrayPos] = topRight;
	(*arrayPos)++;
	vtx[*arrayPos] = bottomLeft;
	(*arrayPos)++;
	vtx[*arrayPos] = bottomRight;
	(*arrayPos)++;
}

void doCarpet(vertex* vtx, int* arrayPos, intrect bnd, int level) {
	if (level == depthLevel) {
		//draw square
		addSquare(vtx, arrayPos, bnd);
	} else {
		//recurse into 8 parts
		intrect child;
		int newW = bnd.w / 3;
		int newH = bnd.h / 3;
		child.w = newW;
		child.h = newH;

		//topleft
		child.x = bnd.x;
		child.y = bnd.y;
		doCarpet(vtx, arrayPos, child, level + 1);

		//left
		child.x = bnd.x;
		child.y = bnd.y + newH;
		doCarpet(vtx, arrayPos, child, level + 1);

		//bottomleft
		child.x = bnd.x;
		child.y = bnd.y + newH + newH;
		doCarpet(vtx, arrayPos, child, level + 1);

		//top
		child.x = bnd.x + newW;
		child.y = bnd.y;
		doCarpet(vtx, arrayPos, child, level + 1);
		
		//bottom
		child.x = bnd.x + newW;
		child.y = bnd.y + newH + newH;
		doCarpet(vtx, arrayPos, child, level + 1);
		
		//topright
		child.x = bnd.x + newW + newW;
		child.y = bnd.y;
		doCarpet(vtx, arrayPos, child, level + 1);
		
		//right
		child.x = bnd.x + newW + newW;
		child.y = bnd.y + newH;
		doCarpet(vtx, arrayPos, child, level + 1);
		
		//bottomright
		child.x = bnd.x + newW + newW;
		child.y = bnd.y + newH + newH;
		doCarpet(vtx, arrayPos, child, level + 1);
	}
}

static void regenVbo() {
	//top, left, right
	vertex vtx[vtx_count];
	int arrayPos = 0;
	const intrect bounding = { rightX, 0, 240, 240 };
	doCarpet(vtx, &arrayPos, bounding, 0);
	
	if (current_vbo_count != vtx_count)
	{
		linearFree(vbo_data);
		vbo_data = linearAlloc(vtx_count * sizeof(vertex));
		current_vbo_count = vtx_count;
	}
	
	memcpy(vbo_data, vtx, sizeof(vtx));

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
	Mtx_OrthoTilt(&projection, 0.0, 400.0, 240.0, 0.0, 0.0, 1.0);

	// Create the VBO (vertex buffer object)
	regenVbo();

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
			rightX += 2.0f;
			dirty = true;
		}
		if (kHeld & KEY_LEFT)
		{
			rightX -= 2.0f;
			dirty = true;
		}
		if (kDown & KEY_A)
		{
			depthLevel++;
			dirty = true;
		}
		if (kDown & KEY_B)
		{
			depthLevel--;
			if (depthLevel < 0)
				depthLevel = 0;
			dirty = true;
		}

		// Render the scene
		if (dirty)
			regenVbo();

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C3D_FrameDrawOn(target);
			sceneRender();
		C3D_FrameEnd(0);
	}

	// Deinitialize the scene
	sceneExit();

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();
	return 0;
}
