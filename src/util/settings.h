#ifndef _SETTINGS_HEADER_
#define _SETTINGS_HEADER_


namespace GSettings {
	void SetSensitivity(float sens);
	float GetSensitivity();

	void SetResolution(int x, int y);
	int GetResolutionW();
	int GetResolutionH();

	int GetResCount();
	int GetResIndex();
	int GetResX(int index);
	int GetResY(int index);
	wchar_t* GetResName(int index);
}



#endif // _SETTINGS_HEADER_