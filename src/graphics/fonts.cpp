#include "graphics/graphics.h"

#include <GL/glew.h>
#include <SDL.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <map>
using namespace std;

namespace GraphicsM{
	TFont::TFont(const char* fontName, unsigned int h){
		printf("Loaded font: %s%d\n", fontName, h);
		unsigned int* textures = new unsigned int[128];
		FT_Library library;
		FT_Init_FreeType(&library);

		FT_Face face;
		FT_New_Face(library, fontName, 0, &face);

		FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

		glGenTextures(128, textures);

		for(unsigned char i=0; i<128; i++){
			FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT);

			FT_Glyph glyph;
			FT_Get_Glyph(face->glyph, &glyph);
			FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
			FT_BitmapGlyph btGlyph = FT_BitmapGlyph(glyph);
			FT_Bitmap& bitmap=btGlyph->bitmap;
			int nWidth = next_p2(bitmap.width);
			int nHeight = next_p2(bitmap.rows);
			glyphs[i].tWidth = nWidth;
			glyphs[i].tHeight = nHeight;
			glyphs[i].width = bitmap.width;
			glyphs[i].height = bitmap.rows;
			unsigned char* pixelData = new unsigned char[nWidth*nHeight*4];
			for(int y=0; y<nHeight; y++){
				for(int x=0; x<nWidth; x++){
					unsigned char nData;
					if(x>=bitmap.width || y>=bitmap.rows){
						nData=0;
					} else {
						nData=bitmap.buffer[x+(bitmap.width*y)];
					}
					int index = 4*(x+y*nWidth);
					pixelData[index]=255;
					pixelData[index+1]=255;
					pixelData[index+2]=255;
					pixelData[index+3]=nData;
				}
			}
			glyphs[i].texID=textures[i];

			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
			delete[] pixelData;
			glyphs[i].left = btGlyph->left;
			glyphs[i].top = btGlyph->top;
			glyphs[i].advance = face->glyph->advance.x >> 6;

		}
		delete[] textures;
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		for(int i=0; i<6; i++){
			drawVerts[i].z=0.0f;
			drawVerts[i].w=1.0f;
		}

		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		glGenBuffers(1, &vBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(drawVerts), drawVerts, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	TFont::~TFont(){
		unsigned int* textures = new unsigned int[128];
		for(int i=0; i<128; i++){
			textures[i]=glyphs[i].texID;
		}
		glDeleteTextures(128, textures);
		delete[] textures;
		glDeleteBuffers(1, &vBuffer);
	}

	void TFont::RenderText(float x, float y, const char* text, Colour col){
		GraphicsM::DisableDepth();
		GraphicsM::EnableAlpha();
		glBindVertexArray(vaoID);
		ShaderProgram* shader = GetShader("default");
		int imageLoc = shader->GetUniformID("baseImage");
		int sizeLoc = shader->GetUniformID("scrSize");
		UseShader(shader);

		glUniform1i(imageLoc, 0);
		glUniform2f(sizeLoc, ScrW(), ScrH());
		glActiveTexture(GL_TEXTURE0);

		//UseShader(shader);
		glEnable(GL_TEXTURE_2D);
		//glDisable(GL_TEXTURE_2D);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexD), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexD), (void*)16);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexD), (void*)32);
		

		for(int i=0; i<6; i++){
			drawVerts[i].r=col.GetFR();
			drawVerts[i].g=col.GetFG();
			drawVerts[i].b=col.GetFB();
			drawVerts[i].a=col.GetFA();
		}
		int chars = strlen(text);
		float offX = 0;
		float offY = 0;
		int maxTop=0;
		for(int i=0; i<chars; i++){
			GlyphData& gData = glyphs[text[i]];
			if(gData.top>maxTop){
				maxTop=gData.top;
			}
		}
		//FontSize tSize = MeasureText(text);
		for(int i=0; i<chars; i++){
			GlyphData& gData = glyphs[text[i]];
			float mU = float(gData.width) / float(gData.tWidth);
			float mV = float(gData.height) / float(gData.tHeight);
			offX += gData.left;
			offY = maxTop-gData.top;
			float zX=x+offX;
			float zY=y+offY;
			Set2DVertex(drawVerts[0], zX, zY, 0, 0);
			Set2DVertex(drawVerts[1], zX+gData.width, zY, mU, 0);
			Set2DVertex(drawVerts[2], zX, zY+gData.height, 0, mV);

			Set2DVertex(drawVerts[3], zX+gData.width, zY, mU, 0);
			Set2DVertex(drawVerts[4], zX+gData.width, zY+gData.height, mU, mV);
			Set2DVertex(drawVerts[5], zX, zY+gData.height, 0, mV);

			/*Set2DVertex(drawVerts[0], 0, 0, 0, 0);
			Set2DVertex(drawVerts[1], gData.width, 0, mU, 0);
			Set2DVertex(drawVerts[2], 0, gData.height, 0, mV);

			Set2DVertex(drawVerts[3], gData.width, 0, mU, 0);
			Set2DVertex(drawVerts[4], gData.width, gData.height, mU, mV);
			Set2DVertex(drawVerts[5], 0, gData.height, 0, mV);*/

			offX += gData.advance;

			glBindTexture(GL_TEXTURE_2D, gData.texID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 6*sizeof(VertexD), &drawVerts[0].x);

			glDrawArrays(GL_TRIANGLES, 0, (GLsizei) 6);

		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glUseProgram(0);
		//glFlush();

	}


	map<string, TFont*> gFonts;

	TFont* LoadFont(const char* fontName, unsigned int h){
		char buffer[50];
		TFont* nFont = new TFont(fontName, h);
		sprintf(buffer, "%s%d", fontName, h);
		gFonts[buffer]=nFont;
		return nFont;
	}

	void FreeFont(const char* fontName, unsigned int h){
		char buffer[50];
		sprintf(buffer, "%s%d", fontName, h);
		map<string, TFont*>::iterator it=gFonts.find(buffer);
		if(it!=gFonts.end()){
			delete it->second;
			gFonts.erase(it);
		}
	}

	TFont* GetFont(const char* fontName, unsigned int h){
		char buffer[50];
		sprintf(buffer, "%s%d", fontName, h);
		map<string, TFont*>::iterator it=gFonts.find(buffer);
		if(it!=gFonts.end()){
			return it->second;
		}
		return NULL;
	}

	FontSize TFont::MeasureText(const char* text){
		int chars = strlen(text);
		int width = 0;
		int height = 0;
		int maxTop = 0;
		for(int i=0; i<chars; i++){
			GlyphData& gData = glyphs[text[i]];
			if(gData.top>maxTop){
				maxTop=gData.top;
			}
		}
		for(int i=0; i<chars; i++){
			GlyphData& gData = glyphs[text[i]];
			width+=gData.left;
			width+=gData.advance;
			int mHeight = (maxTop-gData.top)+gData.height;
			if(mHeight>height){
				height=mHeight;
			}
		}
		FontSize fSize;
		fSize.w=width;
		fSize.h=height;
		return fSize;
	}

	FontSize MeasureFont(TFont* font, const char* text){
		return font->MeasureText(text);
	}
}