
#include "Gwen/Renderers/OpenGL.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"

#include <math.h>
#include <string>

#include "GL/glew.h"

#include "graphics/graphics.h"

unsigned int vao;

namespace Gwen
{
	namespace Renderer
	{
		OpenGL::OpenGL()
		{
			m_iVertNum = 0;

			//::FreeImage_Initialise();
			

			for ( int i=0; i<MaxVerts; i++ )
			{
				m_Vertices[ i ].z = 0.0f;
				m_Vertices[ i ].w = 1.0f;
			}

			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

		}

		OpenGL::~OpenGL()
		{
			//::FreeImage_DeInitialise();
			glDeleteBuffers(1, &vertexBuffer);
		}

		void OpenGL::Init(){

		}

		void OpenGL::Begin()
		{
			glBindVertexArray(vao);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			//glAlphaFunc( GL_GREATER, 1.0f );	
			glEnable ( GL_BLEND );
		}

		void OpenGL::End()
		{
			Flush();
		}

		void OpenGL::Flush()
		{
			if ( m_iVertNum == 0 ) return;

			GraphicsM::UseShader("default");

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_iVertNum*sizeof(Vertex), &m_Vertices[0].x);
			
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16);

			//glVertexPointer( 3, GL_FLOAT,  sizeof(Vertex), (void*) &m_Vertices[0].x );
			//glEnableClientState( GL_VERTEX_ARRAY );

			//glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)&m_Vertices[0].r );
			//glEnableClientState( GL_COLOR_ARRAY );

			//glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (void*) &m_Vertices[0].u );
			//glEnableClientState( GL_TEXTURE_COORD_ARRAY );

			glDrawArrays( GL_TRIANGLES, 0, (GLsizei) m_iVertNum );

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glUseProgram(0);

			m_iVertNum = 0;
			glFlush();
		}

		void OpenGL::AddVert( int x, int y, float u, float v )
		{
			if ( m_iVertNum >= MaxVerts-1 )
			{
				Flush();
			}

			m_Vertices[ m_iVertNum ].x = (float)x;
			m_Vertices[ m_iVertNum ].y = (float)y;
			m_Vertices[ m_iVertNum ].u = u;
			m_Vertices[ m_iVertNum ].v = v;

			m_Vertices[ m_iVertNum ].r = float(m_Color.r)/256.0;
			m_Vertices[ m_iVertNum ].g = float(m_Color.g)/256.0;
			m_Vertices[ m_iVertNum ].b = float(m_Color.b)/256.0;
			m_Vertices[ m_iVertNum ].a = float(m_Color.a)/256.0;

			m_iVertNum++;
			//printf("Added Vertex\n");
		}

		void OpenGL::DrawFilledRect( Gwen::Rect rect )
		{
			GLboolean texturesOn;

			glGetBooleanv(GL_TEXTURE_2D, &texturesOn);
			if ( texturesOn )
			{
				Flush();
				glDisable(GL_TEXTURE_2D);
			}	

			Translate( rect );

			AddVert( rect.x, rect.y );
			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x, rect.y + rect.h );

			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x+rect.w, rect.y+rect.h );
			AddVert( rect.x, rect.y + rect.h );
		}

		void OpenGL::SetDrawColor(Gwen::Color color)
		{
			//glColor4ubv( (GLubyte*)&color );
			m_Color = color;
		}

		void OpenGL::StartClip()
		{
			Flush();
			Gwen::Rect rect = ClipRegion();

			// OpenGL's coords are from the bottom left
			// so we need to translate them here.
			{
				GLint view[4];
				glGetIntegerv( GL_VIEWPORT, &view[0] );
				rect.y = view[3] - (rect.y + rect.h);
			}

			glScissor( rect.x * Scale(), rect.y * Scale(), rect.w * Scale(), rect.h * Scale() );
			glEnable( GL_SCISSOR_TEST );
		};

		void OpenGL::EndClip()
		{
			Flush();
			glDisable( GL_SCISSOR_TEST );
			
		};

		void OpenGL::DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 )
		{
			GraphicsM::MTexture* nTex = (GraphicsM::MTexture*)pTexture->data;
			GLuint tex = nTex->GetTexName();

			// Missing image, not loaded properly?
			if ( !tex )
			{
				return DrawMissingImage( rect );
			}

			Translate( rect );
			GLuint boundtex;

			GLboolean texturesOn;
			glGetBooleanv(GL_TEXTURE_2D, &texturesOn);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint *)&boundtex);
			if ( !texturesOn || tex != boundtex )
			{
				Flush();
				GraphicsM::ShaderProgram* shader = GraphicsM::GetShader("default");
				int imageLoc = shader->GetUniformID("baseImage");
				GraphicsM::UseShader(shader);
				glUniform1i(imageLoc, 0);
				glActiveTexture(GL_TEXTURE0);

				glBindTexture( GL_TEXTURE_2D, tex );
				glEnable(GL_TEXTURE_2D);
			}		

			AddVert( rect.x, rect.y,			u1, v1 );
			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x, rect.y + rect.h,	u1, v2 );

			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x+rect.w, rect.y+rect.h, u2, v2 );
			AddVert( rect.x, rect.y + rect.h, u1, v2 );			
		}

		void OpenGL::LoadTexture( Gwen::Texture* pTexture )
		{
			const char *wFileName = pTexture->name.Get().c_str();

			/*FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileTypeU( wFileName );

			if ( imageFormat == FIF_UNKNOWN )
				imageFormat = FreeImage_GetFIFFromFilenameU( wFileName );

			// Image failed to load..
			if ( imageFormat == FIF_UNKNOWN )
			{
				pTexture->failed = true;
				return;
			}

			// Try to load the image..
			FIBITMAP* bits = FreeImage_LoadU( imageFormat, wFileName );
			if ( !bits )
			{
				pTexture->failed = true;
				return;
			}

			// Convert to 32bit
			FIBITMAP * bits32 = FreeImage_ConvertTo32Bits( bits );
			FreeImage_Unload( bits );
			if ( !bits32 )
			{
				pTexture->failed = true;
				return;
			}

			// Flip
			::FreeImage_FlipVertical( bits32 );


			// Create a little texture pointer..
			GLuint* pglTexture = new GLuint;

			// Sort out our GWEN texture
			pTexture->data = pglTexture;
			pTexture->width = FreeImage_GetWidth( bits32 );
			pTexture->height = FreeImage_GetHeight( bits32 );

			// Create the opengl texture
			glGenTextures( 1, pglTexture );
			glBindTexture( GL_TEXTURE_2D, *pglTexture );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			#ifdef FREEIMAGE_BIGENDIAN
			GLenum format = GL_RGBA;
			#else
			GLenum format = GL_BGRA;
			#endif

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pTexture->width, pTexture->height, 0, format, GL_UNSIGNED_BYTE, (const GLvoid*)FreeImage_GetBits( bits32 ) );

			FreeImage_Unload( bits32 );*/

			GraphicsM::MTexture* nTex = new GraphicsM::MTexture(wFileName, false);
			pTexture->data = nTex;
			pTexture->width = nTex->GetSize().x;
			pTexture->height = nTex->GetSize().y;
		}

		void OpenGL::FreeTexture( Gwen::Texture* pTexture )
		{
			GraphicsM::MTexture* nTex = static_cast<GraphicsM::MTexture*>(pTexture->data);
			delete nTex;
			/*GLuint* tex = (GLuint*)pTexture->data;
			if ( !tex ) return;

			glDeleteTextures( 1, tex );
			delete tex;
			pTexture->data = NULL;*/
		}

		Gwen::Color OpenGL::PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default )
		{
			GLuint* tex = (GLuint*)pTexture->data;
			if ( !tex ) return col_default;

			unsigned int iPixelSize = sizeof(unsigned char) * 4;

			glBindTexture( GL_TEXTURE_2D, *tex );

			unsigned char* data = (unsigned char*) malloc( iPixelSize * pTexture->width * pTexture->height );

				glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

				unsigned int iOffset = (y * pTexture->width + x) * 4;

				Gwen::Color c;
				c.r = data[0 + iOffset];
				c.g = data[1 + iOffset];
				c.b = data[2 + iOffset];
				c.a = data[3 + iOffset];

			//
			// Retrieving the entire texture for a single pixel read
			// is kind of a waste - maybe cache this pointer in the texture
			// data and then release later on? It's never called during runtime
			// - only during initialization.
			//
			free( data );

			return c;
		}

		struct TFontData{
			bool valid;
			std::string fontName;
			int size;
			GraphicsM::TFont* fontLoc;
			TFontData(){
				valid=false;
				fontLoc=NULL;
				size=0;
			}
		};

		void OpenGL::LoadFont(Gwen::Font* pFont){
			int len=pFont->facename.length();
			char* fName = new char[len+1];
			fName[len]=0;
			wcstombs(fName, pFont->facename.c_str(), len);
			GraphicsM::TFont* nFont = GraphicsM::LoadFont(fName, pFont->size);
			TFontData* nData = new TFontData();
			pFont->data = nData;
			nData->valid=true;
			nData->fontName = fName;
			nData->size = pFont->size;
			nData->fontLoc = nFont;
			delete[] fName;
		}

		void OpenGL::FreeFont(Gwen::Font* pFont){
			//printf("How about here?\n");
			if(!pFont->data){
				return;
			}
			TFontData* nData=(TFontData*)pFont->data;
			GraphicsM::FreeFont(nData->fontName.c_str(), nData->size);
			delete nData;
			pFont->data = NULL;
		}

		void OpenGL::RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text){
			if(!pFont->data){
				LoadFont(pFont);
			}

			int len=text.length();
			char* fText = new char[len+1];
			fText[len]=0;
			wcstombs(fText, text.c_str(), len);
			
			TFontData* nData=(TFontData*)pFont->data;
			Translate(pos.x, pos.y);
			nData->fontLoc->RenderText(pos.x, pos.y, fText, GraphicsM::Colour(m_Color.r, m_Color.g, m_Color.b, 255));
			delete[] fText;
		}

		Gwen::Point OpenGL::MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString& text){
			if(!pFont->data){
				LoadFont(pFont);
			}
			TFontData* nData=(TFontData*)pFont->data;

			int len=text.length();
			char* fText = new char[len+1];
			fText[len]=0;
			wcstombs(fText, text.c_str(), len);

			GraphicsM::FontSize fSize = GraphicsM::MeasureFont(nData->fontLoc, fText);
			delete[] fText;
			//printf("Found Size: %d %d\n", fSize.w, fSize.h);
			return Gwen::Point(fSize.w, fSize.h);
		}

	}
}