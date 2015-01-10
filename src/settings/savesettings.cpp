#include "settings/savesettings.h"
#include <map>
#include <fstream>

using namespace std;

map<string, string> gStoredSettings;

string GSaveSettings::GetSetting(string setting){
	map<string, string>::iterator it;
	it=gStoredSettings.find(setting);
	if(it==gStoredSettings.end()){
		return "";
	}
	return it->second;
}

void GSaveSettings::SetSetting(string toSet, string setTo){
	gStoredSettings[toSet] = setTo;
}

void GSaveSettings::LoadSettings(){
	string lineRead;
	ifstream toRead ("settings.txt");
	if (!toRead.is_open()){
		return;
	}
	while(toRead.good()){
		getline(toRead, lineRead);
		size_t found=lineRead.find_first_of(':');
		if(found != string::npos){
			string settingName = lineRead.substr(0, found);
			string settingValue = lineRead.substr(found+2);
			printf("Setting: (%s)=(%s)\n", settingName.c_str(), settingValue.c_str());
			GSaveSettings::SetSetting(settingName, settingValue);
		}
	}
	toRead.close();
}

void GSaveSettings::SaveSettings(){
	ofstream toWrite("settings.txt", ios::out | ios::trunc);
	if(!toWrite.is_open()){
		printf("Could not open file settings.txt.\n");
		return;
	}
	map<string, string>::iterator it;
	for(it=gStoredSettings.begin(); it!=gStoredSettings.end(); it++){
		toWrite << it->first.c_str();
		toWrite << ": ";
		toWrite << it->second.c_str();
		toWrite << "\n";
	}
}