#ifndef _SAVE_SETTINGS_HEADER_
#define _SAVE_SETTINGS_HEADER_

#include <string>

namespace GSaveSettings {
	std::string GetSetting(std::string setting);
	void SetSetting(std::string toSet, std::string setTo);
	void LoadSettings();
	void SaveSettings();
}





#endif // _SETTINGS_HEADER_