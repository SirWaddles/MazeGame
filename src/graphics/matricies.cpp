#include "graphics/graphics.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/core/func_geometric.hpp>

namespace GraphicsM{
	glm::mat4 projection;
	glm::mat4 modelView;
	glm::mat3 nModelView;

	void CreateIsometricMatrix(){
		projection = glm::ortho(-1.0f, 1.0f, 0.0f, 1.0f, 0.005f, 20.0f);
		//projection = glm::rotate(projection, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//projection = glm::rotate(projection, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void CreatePerspectiveMatrix(){
		float ratio = float(ScrW())/float(ScrH());
		projection = glm::perspective(70.0f, ratio, 0.005f, 20.0f);
		//projection = glm::frustum(-35.0f, 35.0f, -35.0f, 35.0f, 1.0f, 35.0f);
	}

	void LoadMIdentity(){
		modelView = glm::mat4x4();
	}

	void TranslateM(float x, float y, float z){
		modelView = glm::translate(modelView, glm::vec3(x, y, z));
	}

	void RotateM(float ang, float xa, float ya, float za){
		modelView = glm::rotate(modelView, ang, glm::vec3(xa, ya, za));
		
	}

	void PrintModelViewMatrix(){
		glm::mat4 toMat = modelView;
		printf("Matrix Start.\n");
		printf("[ %f %f %f %f ]\n", toMat[0].r, toMat[0].g, toMat[0].b, toMat[0].a);
		printf("[ %f %f %f %f ]\n", toMat[1].r, toMat[1].g, toMat[1].b, toMat[1].a);
		printf("[ %f %f %f %f ]\n", toMat[2].r, toMat[2].g, toMat[2].b, toMat[2].a);
		printf("[ %f %f %f %f ]\n", toMat[3].r, toMat[3].g, toMat[3].b, toMat[3].a);
		printf("Matrix end\n");
	}

	void ScaleM(float x, float y, float z){
		modelView = glm::scale(modelView, glm::vec3(x, y, z));
	}

	void UpdateNormalMat(){
		nModelView = glm::transpose(glm::inverse(glm::mat3(modelView)));
		//nModelView = glm::mat3(modelView);
	}

	float* GetProjectionMatrix(){
		return glm::value_ptr(projection);
	}

	float* GetModelViewMatrix(){
		return glm::value_ptr(modelView);
	}

	float* GetNMatrix(){
		return glm::value_ptr(nModelView);
	}

	Vec3 GetCSpace(Vec3 mSpace){
		glm::vec4 nVec(mSpace.x, mSpace.y, mSpace.z, 1.0f);
		//nVec=(glm::inverse(modelView)*nVec);
		nVec=modelView*nVec;
		return Vec3(nVec.x, nVec.y, nVec.z);
	}

	float lastW;

	Vec3 GetSSpace(Vec3 mSpace){
		glm::vec4 nVec(mSpace.x, mSpace.y, mSpace.z, 1.0f);
		nVec = projection * (modelView * nVec);
		float x = ((nVec.x/nVec.w)+1.0f)*float(GraphicsM::ScrW())*0.5f;
		float y = (((nVec.y/nVec.w))+1.0f)*float(GraphicsM::ScrH())*0.5f;
		float z = (nVec.z / nVec.w) * 0.5f + 0.5f;
		lastW = nVec.w;
		//printf("Z: %f W: %f\n", nVec.z, nVec.w);
		return Vec3(x, y, z);
	}

	float GetLastW(){
		return lastW;
	}

	void RotateM(Quaternion quat){
		glm::quat toAng(quat.w, quat.x, quat.y, quat.z);
		glm::mat4 toMat = glm::mat4_cast(toAng);
		modelView = modelView*toMat;
		//modelView = toMat*modelView;
	}

	void DeRotateM(Quaternion quat){
		glm::quat toAng(quat.w, quat.x, quat.y, quat.z);
		glm::mat4 toMat = glm::mat4_cast(toAng);
		modelView = modelView*glm::inverse(toMat);
		//modelView = glm::inverse(toMat)*modelView;
	}

	Quaternion::Quaternion(){
		x=0.0f;
		y=0.0f;
		z=0.0f;
		w=1.0f;
	}

	void Vec2::Normalize(){
		glm::vec2 tVec(x, y);
		tVec = glm::normalize(tVec);
		x=tVec.x;
		y=tVec.y;
	}

	float Vec2::DotProduct(Vec2 &b){
		glm::vec2 tVec(x,y);
		glm::vec2 tVec2(b.x, b.y);
		float ret = glm::dot(tVec, tVec2);
		return ret;
	}

	float Vec3::Magnitude(){
		return sqrt(x*x + y*y + z*z);
	}

	float Vec3::Distance(Vec3 &b){
		return (b-*this).Magnitude();
	}

	void Vec3::Normalize(){
		float nMag = this->Magnitude();
		x=x/nMag;
		y=y/nMag;
		z=z/nMag;
	}

	void Quaternion::Normalize(){
		glm::quat toAng(w, x, y, z);
		toAng = glm::normalize(toAng);
		w = toAng.w;
		x = toAng.x;
		y = toAng.y;
		z = toAng.z;
	}

}