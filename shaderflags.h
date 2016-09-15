//-----------------------------------------------------------------------------
// ShaderFlags
//-----------------------------------------------------------------------------

#ifndef YOL_SHADERFLAGS_H
#define YOL_SHADERFLAGS_H

/**
 * Transform functions.
 */
enum FUNC
{
    FUNC_CONST,
	FUNC_SIN,
    FUNC_TRIANGLE,
    FUNC_SQUARE,
    FUNC_SAWTOOTH,
    FUNC_INVERSESAWTOOTH,
	FUNC_NOISE
};

/**
 * Functions for generating and deforming vertexes.
 */
enum RGBGEN
{
	RGBGEN_NONE,
	RGBGEN_IDENTITY_LIGHTING,	/**< 0 */
	RGBGEN_IDENTITY,			/**< 0 */
	RGBGEN_WAVE,				/**< 5 */
	RGBGEN_ENTITY,				/**< 0 */
	RGBGEN_ONE_MINUS_ENTITY,	/**< 0 */
	RGBGEN_VERTEX,				/**< 0 */
	RGBGEN_EXACTVERTEX,
	RGBGEN_ONE_MINUS_VERTEX,	/**< 0 */
	RGBGEN_LIGHTING_DIFFUSE,	/**< 0 */
	RGBGEN_FLARE,
	RGBGEN_FOG,
	RGBGEN_CONST,
	RGBGEN_MATERIAL
};

/**
 * Alpha channel generation.
 */
enum ALPHAGEN
{
	ALPHAGEN_NONE,
	ALPHAGEN_PORTAL,
	ALPHAGEN_IDENTITY,
	ALPHAGEN_CONST,
	ALPHAGEN_VERTEX,
	ALPHAGEN_ENTITY,
	ALPHAGEN_SPECULAR,
	ALPHAGEN_WAVE
};

/**
 * Texture coordinate generation.
 */
enum TCGEN
{
	TCGEN_BASE,					/**< 0 */
	TCGEN_ENVIRONMENT,			/**< 0 */
	TCGEN_LIGHTMAP,				/**< 0 */
	TCGEN_VECTOR,				/**< 6 */
	TCGEN_FOG
};

/**
 * Texture coordinate deformation.
 */
enum TCMOD
{
	SP_TCMOD_NONE		= 0,
	TCMOD_ROTATE		= 1,
	TCMOD_SCALE			= 2,
	TCMOD_SCROLL		= 3,
	TCMOD_STRETCH		= 4,
	TCMOD_TRANSFORM		= 5,
	TCMOD_TURB			= 6,
};

/**
 * Vertes coordinates deformations.
 * @see DeformVertexes
 */
enum DEFORMVERTEXES
{
	DEFORMVERTEXES_NONE			= 0,	/**< 0 parameters */
	DEFORMVERTEXES_WAVE			= 1,	/**< 6 parameters */
	DEFORMVERTEXES_NORMAL		= 2,	/**< 5 parameters */
	DEFORMVERTEXES_BULGE		= 3,	/**< 3 parameters */
	DEFORMVERTEXES_MOVE			= 4,	/**< 8 parameters */
	DEFORMVERTEXES_AUTOSPRITE	= 5,	/**< 0 parameters */
	DEFORMVERTEXES_AUTOSPRITE2	= 6,	/**< 0 parameters */
};

/**
 * Layer flags.
 */
enum
{
	LF_NOMIPMAP		= 0x01,		/**< do not create mipmaps */
	LF_NOPICMIP		= 0x02,		/**< allways pick the highest mipmap */
	LF_CLAMP		= 0x04,		/**< clamp, default is repeat */
	LF_DETAIL		= 0x08,		/**< detail texture */
	LF_LIGHTMAP		= 0x10,		/**< lightmap */
	LF_HASTCMOD		= 0x20,
	LF_USENORMALS	= 0x40,
	LF_MULTITEX		= 0x80		/**< this shader and the next could be collapsed */
};

enum
{																	// val << 16
	SORT_PORTAL			= 1,										// 0x10000
	SORT_SKY			= 2,										// 0x20000
	SORT_NORMAL			= 3, SORT_OPAQUE = 3,						// 0x30000
	SORT_DECAL			= 4,										// 0x40000
	SORT_UNDERBANNER	= 5,										// 0x50000
	SORT_BANNER			= 6,										// 0x60000
	SORT_TRANS			= 7, SORT_SEETHROUGH = 7,					// 0x70000
	SORT_UNDERWATER		= 8,										// 0x80000
	SORT_ADDITIVE		= 9,										// 0x90000
	SORT_ADDITIVE1		= 10,										// 0xA0000
	SORT_ADDITIVE2		= 11,										// 0xB0000
	SORT_FOG			= 12,										// 0xC0000
	SORT_FOG1			= 13,										// 0xD0000
	SORT_FOG2			= 14,										// 0xE0000
	SORT_STENCILSHADOW	= 15,										// 0xF0000
	SORT_NEAREST		= 16,										// 0x100000
};

/**
 * Culling type.
 */
enum
{
	CULL_FRONT	= 1,
	CULL_BACK	= -1,
	CULL_NONE	= 0,
};

/**
 * Shader flags.
 */
enum
{
	SF_NOMIPMAP			= (1<<1),	/**< do not create mipmaps */
	SF_NOPICMIP			= (1<<2),	/**< allways pick the highest mipmap */
	SF_POLYGONOFFSET	= (1<<3),	/**< surface needs polygon offset */
	SF_PORTAL			= (1<<4),	/**< there is a portal layer */
	SF_HASLIGHTMAP		= (1<<5),	/**< there is a lightmap layer */
	SF_HASCOLORS		= (1<<6),	/**< there is a layer that needs colors */
	SF_HASTCMOD			= (1<<7),	/**< there is a layer that has lf_hastcmod */
	SF_USENORMALS		= (1<<8),	/**< there is a layer that has lf_usenormals */
	SF_MULTITEX			= (1<<9),	/**< this shader has two layers that can be collapsed */
};

/**
 * Face types.
 */
enum
{
	FACETYPE_NORMAL	= 1,
	FACETYPE_PLANAR	= 1,		/**< the same as above */
	FACETYPE_PATCH	= 2,
	FACETYPE_MESH	= 3,
	FACETYPE_FLARE	= 4
};

#endif	//YOL_SHADERFLAGS_H
