#include "graphics/graphics.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SOIL.h>

#include <cmath>
#include <cassert>

int isPowerOfTwo (unsigned int x){
	return ((x != 0) && !(x & (x - 1)));
}

namespace GraphicsM {

	MTexture::MTexture(const char *path, bool invertY){
		/*SDL_Surface *tex=SDL_LoadBMP(path);
		assert(tex!=NULL);
		tW=tex->w;
		tH=tex->h;
		GLuint textureLoc;
		GLenum texture_format;
		GLenum internal_format;
		GLint nOfColors;
		if(!isPowerOfTwo(tex->w) || !isPowerOfTwo(tex->h)){
			return;
		}
		nOfColors=tex->format->BytesPerPixel;
		if(nOfColors==4){
			texture_format=GL_RGBA;
			internal_format = GL_RGBA;
			alpha=true;
		} else if(nOfColors==3){
			texture_format = GL_BGR;
			internal_format = GL_RGB;
			alpha=false;
		} else {
			return;
		}

		glGenTextures(1, &textureLoc);
		glBindTexture(GL_TEXTURE_2D, textureLoc);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, tex->w, tex->h, 0, texture_format, GL_UNSIGNED_BYTE, tex->pixels);
		texLocate=textureLoc;
		SDL_FreeSurface(tex);*/
		unsigned int flags = SOIL_FLAG_TEXTURE_REPEATS;
		if(invertY){
			flags |= SOIL_FLAG_INVERT_Y;
		}
		texLocate = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
		tW = SOIL_get_last_width();
		tH = SOIL_get_last_height();
		printf("W: %i H: %i\n", tW, tH);
	}

	MTexture::~MTexture(){
		GLuint textureLoc=texLocate;
		glDeleteTextures(1, &textureLoc);
	}

	Vec2 MTexture::GetSize(){
		return Vec2(tW, tH);
	}

	unsigned int MTexture::GetTexName(){
		return texLocate;
	}

	Material::Material(){
		matTexture = NULL;
	}

}