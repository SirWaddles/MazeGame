#include "graphics/graphics.h"
#include "graphics/sdlwin.h"
#include "settings/savesettings.h"
#include <GL/glew.h>
#include <SDL.h>
#include "gui/gui.h"


namespace GraphicsM{

	SDL_Window *mainWindow;
	SDL_GLContext mainContext;
	int screenWidth = 800;
	int screenHeight = 600;
	bool fullscreen = false;

	void CreateMWindow(){
		if(SDL_Init(SDL_INIT_VIDEO) < 0){
			printf("Can't initialize. %s\n", SDL_GetError());
			return;
		}
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		//printf("1\n");
		/*SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );*/

		std::string resStr = GSaveSettings::GetSetting("resolution");
		size_t fChar = resStr.find_first_of('x');
		if(fChar != std::string::npos){
			screenWidth = atoi(resStr.substr(0, fChar).c_str());
			screenHeight = atoi(resStr.substr(fChar+1).c_str());
		}
		std::string fullStr = GSaveSettings::GetSetting("fullscreen");
		if(fullStr.compare("true")==0){
			fullscreen=true;
		}

		if(fullscreen){
			mainWindow=SDL_CreateWindow("Mazey Maze of Maze", 0, 0, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
			//printf("2\n");
			//mainWindow=SDL_CreateWindow("The Game.", 0, 0, DEF_SCREEN_W, DEF_SCREEN_H, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
		} else {
			mainWindow=SDL_CreateWindow("Mazey Maze of Maze", 40, 40, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		}
		mainContext=SDL_GL_CreateContext(mainWindow);
		//printf("3\n");

		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		//printf("4\n");
		if(GLEW_OK != err){
			printf("No glew: %s\n", glewGetErrorString(err));
		}
		if(GLEW_VERSION_3_3){
			printf("GL 3.3!\n");
		} else {
			printf("Error: No GL\n");
			throw -1;
			return;
		}
		if(GLEW_VERSION_3_0){
			printf("GL 3.0!\n");
		}
		if(GLEW_VERSION_2_1){
			printf("GL 2.1!\n");
		}
		if(GLEW_VERSION_1_3){
			printf("GL 1.3!\n");
		}
		printf("Glew: %s\n", glewGetString(GLEW_VERSION));

		SDL_GL_SetSwapInterval(1);

		glViewport(0,0, screenWidth, screenHeight);
		SetupDepth();

	}

	int ScrW(){
		return screenWidth;
	}

	int ScrH(){
		return screenHeight;
	}

	void SetScreenSize(int sW, int sH, bool full){
		screenWidth = sW;
		screenHeight = sH;
		SDL_SetWindowSize(mainWindow, sW, sH);
		if(full!=fullscreen){
			if(full){
				SDL_SetWindowFullscreen(mainWindow, SDL_TRUE);
			} else {
				SDL_SetWindowFullscreen(mainWindow, SDL_FALSE);
			}
		}
		fullscreen = full;
		CreatePerspectiveMatrix();
		GuiM::SetCanvasSize(sW, sH);
		glViewport(0, 0, ScrW(), ScrH());
	}

	SDL_Window* GetMainWindow(){
		return mainWindow;
	}

	void ResetMouseToCenter(){
		//SDL_WarpMouseInWindow(mainWindow, DEF_SCREEN_W/2, DEF_SCREEN_H/2);
	}

	void EnableDepth(){
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void DisableDepth(){
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	void SetupDepth(){
		glCullFace(GL_BACK);
		//glFrontFace(GL_CW);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		//glDepthRange(0.0f, 1.0f);
		glClearDepth(1.0f);
	}

	void EnableAlpha(){
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}

	void DisableAlpha(){
		glDisable(GL_BLEND);
	}

	void DestroyWindow(){
		SDL_GL_DeleteContext(mainContext);
		SDL_DestroyWindow(mainWindow);
		mainWindow=NULL;
		SDL_Quit();
	}

	void SwapBuffers(){
		SDL_GL_SwapWindow(mainWindow);
	}

	void ClearWindow(Colour col){
		glClearColor(col.r, col.g, col.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ClearDepth(){
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	float Colour::GetFR(){
		return r;
	}

	float Colour::GetFG(){
		return g;
	}

	float Colour::GetFB(){
		return b;
	}

	float Colour::GetFA(){
		return a;
	}

	void Set2DVertex(VertexD &oldV, float x, float y, float u, float v){
		oldV.x=x;
		oldV.y=y;
		oldV.u=u;
		oldV.v=v;
	}

	Light::Light(){
		lType = LIGHT_OMNI;
		lPos = Vec3(0.0f, 0.0f, 0.0f);
		intensity = Colour(0,0,0,255);
		strength = 1.2f;
		shiny = 0.2f;
	}

}