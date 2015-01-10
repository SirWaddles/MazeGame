#include "modules/modules.h"
#include <list>

using namespace std;

namespace Modules{

	list<IModule*> gModules;
	IModule* gFocus=NULL;

	void HookModule(IModule* module){
		list<IModule*>::iterator it;
		for(it=gModules.begin(); it!=gModules.end(); it++){
			if((*it)==module){
				return;
			}
		}
		gModules.push_back(module);
	}

	void RemoveModule(IModule* module){
		list<IModule*>::iterator it;
		for(it=gModules.begin(); it!=gModules.end(); it++){
			if((*it)==module){
				gModules.erase(it);
				if(module==gFocus){
					if(gModules.size()>0){
						gFocus=*gModules.begin();
					} else {
						gFocus=NULL;
					}
				}
				break;
			}
		}
	}

	void SetFocus(IModule* module){
		if(gFocus!=NULL){
			gFocus->DeSelect(module);
		}
		module->Select(gFocus);
		gFocus=module;
	}

	void DoDraw(){
		gFocus->Draw();
	}

	void DoThink(){
		gFocus->Think();
	}

	void DoEvent(SDL_Event &kevent){
		gFocus->Event(kevent);
	}

	void DoBGEvent(SDL_Event &kevent){
		list<IModule*>::iterator it;
		for(it=gModules.begin(); it!=gModules.end(); it++){
			(*it)->BGEvent(kevent);
		}
	}

}