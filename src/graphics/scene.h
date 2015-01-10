#ifndef _SCENE_HEADER_
#define _SCENE_HEADER_

#include "graphics/graphics.h"
#include <list>

#define MAX_LIGHTS 16

/*class Particle{
public:
	GraphicsM::Vec3 Position;
	GraphicsM::Vec3 Scale;
	GraphicsM::Quaternion Angle;

	GraphicsM::Vec3 Direction;
	float Speed;

protected:

};*/

class Scene{
public:
	void DrawScene();
	void AddLight(GraphicsM::Light* light);
	void AddModel(GraphicsM::ModelInstance* model);

	void RemoveModel(GraphicsM::ModelInstance* model);
	void RemoveLight(GraphicsM::Light* light);
	void RemoveAllLights();

	Scene();
	~Scene();
protected:
	std::list<GraphicsM::Light*> lights;
	std::list<GraphicsM::ModelInstance*> models;
};

GraphicsM::Vec3 GetLocalPlayerPosition();




#endif