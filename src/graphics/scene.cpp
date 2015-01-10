#include "graphics/scene.h"
#include <GL/glew.h>

using namespace GraphicsM;
using namespace std;

struct cShaderDets{
	unsigned int projMat;
	unsigned int modelView;
	unsigned int nModelView;

	unsigned int lightIntensity;
	unsigned int cameraSpaceLightPos;
	unsigned int lightAttenuation;
	unsigned int shininessFactor;

	unsigned int diffuseColor;
	unsigned int ambientColor;
	unsigned int specularColor;
	//unsigned int dirToCamera;

	unsigned int baseImage;
};

cShaderDets cShaders;

void Scene::AddLight(Light* light){
	//if(lights.size()>=MAX_LIGHTS) return;
	lights.push_back(light);
}

void Scene::AddModel(ModelInstance* model){
	models.push_back(model);
}

void Scene::RemoveLight(Light* light){
	lights.remove(light);
	delete light;
}

void Scene::RemoveModel(ModelInstance* model){
	models.remove(model);
	delete model;
}

void Scene::RemoveAllLights(){
	std::list<Light*>::iterator it;
	for(it=lights.begin(); it!=lights.end(); it++){
		delete (*it);
	}
	lights.clear();
}

bool CompareDistances(const Light* a, const Light* b){
	return (a->wCheck < b->wCheck);
}

void Scene::DrawScene(){
	EnableDepth();
	GraphicsM::UseShader("colour");
	glUniformMatrix4fv(cShaders.projMat, 1, false, GetProjectionMatrix());
	
	//Vec3 CameraDir(0.0f, 0.0f, 1.0f);
	//CameraDir = GetCSpace(CameraDir);
	//glUniform3f(cShaders.dirToCamera, CameraDir.x, CameraDir.y, CameraDir.z);
	Vec3 plyPos = GetLocalPlayerPosition();
	list<Light*>::iterator lit;
	for(lit=lights.begin(); lit!=lights.end(); lit++){
		
		(*lit)->wCheck = (*lit)->lPos.Distance(plyPos);
	}
	lights.sort(CompareDistances);
	
	Light lightv[MAX_LIGHTS];

	//int nDex=0;
	lit=lights.begin();
	for(int i=0; i<MAX_LIGHTS; i++){
		if(lit==lights.end()) break;
		lightv[i] = (**lit);
		lit++;
	}

	for(int i=0; i<MAX_LIGHTS; i++){
		glUniform4f(cShaders.lightIntensity+i, lightv[i].intensity.GetFR(), lightv[i].intensity.GetFG(), lightv[i].intensity.GetFB(), lightv[i].intensity.GetFA());
		Vec3 lightPos = GetCSpace(lightv[i].lPos);
		//Vec3 lightPos = lightv[i].lPos;
		glUniform3f(cShaders.cameraSpaceLightPos+i, lightPos.x, lightPos.y, lightPos.z);
		glUniform1f(cShaders.lightAttenuation+i, lightv[i].strength);
		glUniform1f(cShaders.shininessFactor+i, lightv[i].shiny);
	}

	//Models
	list<ModelInstance*>::iterator mit;
	for(mit=models.begin(); mit!=models.end(); mit++){
		glUniform4f(cShaders.diffuseColor,
			(*mit)->material->diffuse.GetFR(),
			(*mit)->material->diffuse.GetFG(),
			(*mit)->material->diffuse.GetFB(),
			(*mit)->material->diffuse.GetFA());
		glUniform4f(cShaders.ambientColor,
			(*mit)->material->ambient.GetFR(),
			(*mit)->material->ambient.GetFG(),
			(*mit)->material->ambient.GetFB(),
			(*mit)->material->ambient.GetFA());
		glUniform4f(cShaders.specularColor,
			(*mit)->material->specular.GetFR(),
			(*mit)->material->specular.GetFG(),
			(*mit)->material->specular.GetFB(),
			(*mit)->material->specular.GetFA());

		if((*mit)->material->matTexture){
			glUniform1i(cShaders.baseImage, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (*mit)->material->matTexture->GetTexName());
			glEnable(GL_TEXTURE_2D);
		}

		TranslateM((*mit)->Position.x, (*mit)->Position.y, (*mit)->Position.z);
		ScaleM((*mit)->Scale.x, (*mit)->Scale.y, (*mit)->Scale.z);
		RotateM((*mit)->Angle);
		
		//printf("Ang: %f %f %f\n", (*mit)->Angle.x, (*mit)->Angle.y, (*mit)->Angle.z);
		
		glUniformMatrix4fv(cShaders.modelView, 1, false, GetModelViewMatrix());
		UpdateNormalMat();
		glUniformMatrix3fv(cShaders.nModelView, 1, false, GetNMatrix());

		//PrintModelViewMatrix();
		(*mit)->model->DrawModel();
		
		DeRotateM((*mit)->Angle);
		ScaleM(1.0f/(*mit)->Scale.x, 1.0f/(*mit)->Scale.y, 1.0f/(*mit)->Scale.z);
		TranslateM(-(*mit)->Position.x, -(*mit)->Position.y, -(*mit)->Position.z);
		
	}
	//DisableDepth();
}

Scene::Scene(){
	ShaderProgram* cShader=GraphicsM::GetShader("colour");
	cShaders.projMat = cShader->GetUniformID("projMat");
	cShaders.modelView = cShader->GetUniformID("modelView");
	cShaders.nModelView = cShader->GetUniformID("nModelView");

	cShaders.lightIntensity = cShader->GetUniformID("lightIntensity[0]");
	
	cShaders.cameraSpaceLightPos = cShader->GetUniformID("cameraSpaceLightPos[0]");
	cShaders.lightAttenuation = cShader->GetUniformID("lightAttenuation[0]");
	cShaders.shininessFactor = cShader->GetUniformID("shininessFactor[0]");
	cShaders.diffuseColor = cShader->GetUniformID("diffuseColor");
	cShaders.ambientColor = cShader->GetUniformID("ambientColor");
	cShaders.specularColor = cShader->GetUniformID("specularColor");
	//cShaders.dirToCamera = cShader->GetUniformID("dirToCamera");

	cShaders.baseImage = cShader->GetUniformID("baseImage");
}

Scene::~Scene(){
	list<Light*>::iterator lit;
	for(lit=lights.begin(); lit!=lights.end(); lit++){
		delete (*lit);
	}
	list<ModelInstance*>::iterator mit;
	for(mit=models.begin(); mit!=models.end(); mit++){
		delete (*mit);
	}
}