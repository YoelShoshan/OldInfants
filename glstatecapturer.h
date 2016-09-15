#ifndef GLSTATECAPTURER_H
#define GLSTATECAPTURER_H

#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "glext.h"
#include "glExtensionsHandling.h"
/*
#define MAX_UNITS_NUM 8

//class CGLStateCapturer : public CSingleton<CGLStateCapturer>
class CGLStateCapturer
{
public:
	
	struct t_GLState
	{
		void GetValuesFromGL(void)
		{
			glGetBooleanv(GL_VERTEX_ARRAY,(GLboolean*)&bGL_VERTEX_ARRAY);
			glGetBooleanv(GL_NORMAL_ARRAY,(GLboolean*)&bGL_NORMAL_ARRAY);
			glGetBooleanv(GL_INDEX_ARRAY,(GLboolean*)&bGL_INDEX_ARRAY);

			glGetBooleanv(GL_BLEND,(GLboolean*)&bGL_BLEND); 
			glGetBooleanv(GL_CULL_FACE,(GLboolean*)&bGL_CULL_FACE);
			glGetBooleanv(GL_DEPTH_TEST,(GLboolean*)&bGL_DEPTH_TEST);
			glGetBooleanv(GL_DEPTH_WRITEMASK,(GLboolean*)&bGL_DEPTH_WRITEMASK);
			glGetBooleanv(GL_DITHER,(GLboolean*)&bGL_DITHER);
			glGetBooleanv(GL_DOUBLEBUFFER,(GLboolean*)&bGL_DOUBLEBUFFER);
			glGetBooleanv(GL_EDGE_FLAG,(GLboolean*)&bGL_EDGE_FLAG);
			glGetBooleanv(GL_EDGE_FLAG_ARRAY,(GLboolean*)&bGL_EDGE_FLAG_ARRAY);
			glGetBooleanv(GL_FOG,(GLboolean*)&bGL_FOG);

			glGetBooleanv(GL_INDEX_LOGIC_OP,(GLboolean*)&bGL_INDEX_LOGIC_OP);
			glGetBooleanv(GL_LIGHTING,(GLboolean*)&bGL_LIGHTING);
			glGetBooleanv(GL_LINE_SMOOTH,(GLboolean*)&bGL_LINE_SMOOTH);

			glGetBooleanv(GL_NORMALIZE,(GLboolean*)&bGL_NORMALIZE);
			glGetBooleanv(GL_RGBA_MODE,(GLboolean*)&bGL_RGBA_MODE);
			glGetBooleanv(GL_SCISSOR_TEST,(GLboolean*)&bGL_SCISSOR_TEST);
			glGetBooleanv(GL_STENCIL_TEST,(GLboolean*)&bGL_STENCIL_TEST);
			glGetBooleanv(GL_STEREO,(GLboolean*)&bGL_STEREO);


			for (int i=0;i<MAX_UNITS_NUM;i++)
			{
				glClientActiveTextureARB(GL_TEXTURE0_ARB+i); 
				glActiveTextureARB(GL_TEXTURE0_ARB+i);		

				glGetBooleanv(GL_TEXTURE_1D,(GLboolean*)&m_TexUnits[i].bGL_TEXTURE_1D);
				glGetBooleanv(GL_TEXTURE_2D,(GLboolean*)&m_TexUnits[i].bGL_TEXTURE_2D);
				glGetBooleanv(GL_TEXTURE_3D,(GLboolean*)&m_TexUnits[i].bGL_TEXTURE_3D);

				glGetBooleanv(GL_TEXTURE_COORD_ARRAY,(GLboolean*)&m_TexUnits[i].bGL_TEXTURE_COORD_ARRAY);
				glGetBooleanv(GL_TEXTURE_CUBE_MAP,(GLboolean*)&m_TexUnits[i].bGL_TEXTURE_CUBE_MAP);

				glGetIntegerv(GL_TEXTURE_BINDING_1D,(GLint*)&m_TexUnits[i].iGL_TEXTURE_BINDING_1D);
				glGetIntegerv(GL_TEXTURE_BINDING_2D,(GLint*)&m_TexUnits[i].iGL_TEXTURE_BINDING_2D);
				glGetIntegerv(GL_TEXTURE_BINDING_3D,(GLint*)&m_TexUnits[i].iGL_TEXTURE_BINDING_3D);

			}

			glGetBooleanv(GL_TEXTURE_GEN_S,(GLboolean*)&bGL_TEXTURE_GEN_S);
			glGetBooleanv(GL_TEXTURE_GEN_T,(GLboolean*)&bGL_TEXTURE_GEN_T);
			glGetBooleanv(GL_TEXTURE_GEN_R,(GLboolean*)&bGL_TEXTURE_GEN_R);
			glGetBooleanv(GL_TEXTURE_GEN_Q,(GLboolean*)&bGL_TEXTURE_GEN_Q);
			glGetBooleanv(GL_ALPHA_TEST,(GLboolean*)&bGL_ALPHA_TEST);
			glGetBooleanv(GL_AUTO_NORMAL,(GLboolean*)&bGL_AUTO_NORMAL);
			glGetBooleanv(GL_COLOR_MATERIAL,(GLboolean*)&bGL_COLOR_MATERIAL);
			glGetBooleanv(GL_COLOR_WRITEMASK,(GLboolean*)&bGL_COLOR_WRITEMASK);
			glGetBooleanv(GL_CONVOLUTION_1D_EXT,(GLboolean*)&bGL_CONVOLUTION_1D_EXT);
			glGetBooleanv(GL_CONVOLUTION_2D_EXT,(GLboolean*)&bGL_CONVOLUTION_2D_EXT);
			glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,(GLboolean*)&bGL_CURRENT_RASTER_POSITION_VALID);
			glGetBooleanv(GL_INDEX_MODE,(GLboolean*)&bGL_INDEX_MODE);
			glGetBooleanv(GL_POLYGON_OFFSET_FILL,(GLboolean*)&bGL_POLYGON_OFFSET_FILL);

			glGetIntegerv(GL_BLEND_DST,(GLint*)&cGL_BLEND_DST);
			glGetIntegerv(GL_BLEND_SRC,(GLint*)&cGL_BLEND_SRC);
			glGetIntegerv(GL_DEPTH_FUNC,(GLint*)&cGL_DEPTH_FUNC);
			glGetIntegerv(GL_STENCIL_BITS,(GLint*)&iGL_STENCIL_BITS);
			glGetIntegerv(GL_STENCIL_WRITEMASK,(GLint*)&iGL_STENCIL_WRITEMASK);

			

			
			glGetIntegerv(GL_STENCIL_FUNC,(GLint*)&cGL_STENCIL_FUNC);
			glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,(GLint*)&cGL_STENCIL_PASS_DEPTH_FAIL);
			glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,(GLint*)&cGL_STENCIL_PASS_DEPTH_PASS);
			

			//glGetFloatv(GL_POLYGON_OFFSET_BIAS,(GLfloat*)&fGL_POLYGON_OFFSET_BIAS);
			glGetFloatv(GL_POLYGON_OFFSET_FACTOR,(GLfloat*)&fGL_POLYGON_OFFSET_FACTOR);
			

			



		}

		struct t_GLTexUnit
		{
			bool bGL_TEXTURE_COORD_ARRAY;
			bool bGL_TEXTURE_CUBE_MAP;

			bool bGL_TEXTURE_1D;
			bool bGL_TEXTURE_2D;
			bool bGL_TEXTURE_3D;

			int iGL_TEXTURE_BINDING_1D;
			int iGL_TEXTURE_BINDING_2D;
			int iGL_TEXTURE_BINDING_3D;

		};

		t_GLTexUnit m_TexUnits[MAX_UNITS_NUM];


		bool bGL_VERTEX_ARRAY;
		bool bGL_NORMAL_ARRAY;
		bool bGL_INDEX_ARRAY;

		bool bGL_BLEND;	
		bool bGL_CULL_FACE;
		bool bGL_DEPTH_TEST;
		bool bGL_DEPTH_WRITEMASK;
		bool bGL_DITHER;
		bool bGL_DOUBLEBUFFER;
		bool bGL_EDGE_FLAG;
		bool bGL_EDGE_FLAG_ARRAY;
		bool bGL_FOG;

		bool bGL_INDEX_LOGIC_OP;
		bool bGL_LIGHTING;
		bool bGL_LINE_SMOOTH;

		bool bGL_NORMALIZE;
		bool bGL_RGBA_MODE;
		bool bGL_SCISSOR_TEST;
		bool bGL_STENCIL_TEST;
		bool bGL_STEREO;
		bool bGL_TEXTURE_GEN_S;
		bool bGL_TEXTURE_GEN_T;
		bool bGL_TEXTURE_GEN_R;
		bool bGL_TEXTURE_GEN_Q;
		bool bGL_ALPHA_TEST;
		bool bGL_AUTO_NORMAL;
		bool bGL_COLOR_MATERIAL;
		bool bGL_COLOR_WRITEMASK;
		bool bGL_CONVOLUTION_1D_EXT;
		bool bGL_CONVOLUTION_2D_EXT;
		bool bGL_CURRENT_RASTER_POSITION_VALID;

		bool bGL_INDEX_MODE;

		bool bGL_POLYGON_OFFSET_FILL;

		int cGL_BLEND_DST;
		int cGL_BLEND_SRC;
		int cGL_DEPTH_FUNC;
		int cGL_STENCIL_FUNC;
		int cGL_STENCIL_PASS_DEPTH_FAIL;
		int cGL_STENCIL_PASS_DEPTH_PASS;
		int iGL_STENCIL_WRITEMASK;

		int iGL_STENCIL_BITS;

		//float fGL_POLYGON_OFFSET_BIAS;
		float fGL_POLYGON_OFFSET_FACTOR;

	};
	CGLStateCapturer();
	~CGLStateCapturer();
	void Capture();
	
	void SetMainWindow(HWND hMainWindow) { if (hMainWindow) m_hMainWindow=hMainWindow; };
	

protected:
	HWND m_hMainWindow;
	t_GLState m_tGLState;


};
*/
#endif