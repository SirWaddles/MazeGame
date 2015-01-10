#include "util/settings.h"
#include "graphics/graphics.h"

namespace GSettings {


	float gSens = 1.0f;

	void SetSensitivity(float sens){
		gSens = sens/10.0f;
	}

	float GetSensitivity(){
		return gSens;
	}

	struct ResVal {
		int sWidth;
		int sHeight;
		wchar_t* lString;
		float aspect;
		ResVal(int sW, int sH, wchar_t* lS, float as) : sWidth(sW), sHeight(sH), lString(lS), aspect(as){

		}
		ResVal(){
			sWidth=0;
			sHeight=0;
			lString=NULL;
			aspect=0.0f;
		}
	};

#define MAX_RES 12

	ResVal GResolutions[MAX_RES];
	bool resBuilt = false;

	int resHeader = 0;

	void BuildResolutions(){
		if(resBuilt){
			return;
		}
		resBuilt = true;
		GResolutions[0] = ResVal(800, 600, L"800x600", 800.0f/600.0f);
		GResolutions[1] = ResVal(1024, 768, L"1024x768", 800.0f/600.0f);
		GResolutions[2] = ResVal(1280, 960, L"1280x960", 800.0f/600.0f);

		GResolutions[3] = ResVal(1280, 1024, L"1280x1024", 1280.0f/1024.0f);

		GResolutions[4] = ResVal(1280, 720, L"1280x720", 1280.0f/720.0f);
		GResolutions[5] = ResVal(1366, 768, L"1366x768", 1280.0f/720.0f);
		GResolutions[6] = ResVal(1600, 900, L"1600x900", 1280.0f/720.0f);
		GResolutions[7] = ResVal(1920, 1080, L"1920x1080", 1280.0f/720.0f);

		GResolutions[8] = ResVal(1280, 800, L"1280x800", 1280.0f/800.0f);
		GResolutions[9] = ResVal(1440, 900, L"1440x900", 1440.0f/900.0f);
		GResolutions[10] = ResVal(1680, 1050, L"1680x1050", 1680.0f/1050.0f);
		GResolutions[11] = ResVal(1920, 1200, L"1920x1200", 1920.0f/1200.0f);
	}

	int GetResCount(){
		BuildResolutions();

		GraphicsM::Vec2 wSize = GraphicsM::GetFullscreenResolution();
		float aspect = wSize.x/wSize.y;
		int count=0;
		for(int i=0; i<MAX_RES; i++){
			if(GResolutions[i].aspect == aspect){
				if(resHeader==0){
					resHeader=i;
				}
				count++;
			}
		}
		return count;
	}

	int GetResIndex(){
		return resHeader;
	}

	int GetResX(int index){
		return GResolutions[index].sWidth;
	}

	int GetResY(int index){
		return GResolutions[index].sHeight;
	}

	wchar_t* GetResName(int index){
		return GResolutions[index].lString;
	}

}