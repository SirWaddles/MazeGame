#include "graphics/graphics.h"
#include "objreader/meshObject.h"

#include <GL/glew.h>
#include <SDL.h>
//#include <SDL_image.h>

using namespace std;

namespace GraphicsM{

	Model::Model(const char* objLoc){
		MeshObject mesh(objLoc);
		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &tBuffer);
		glGenBuffers(1, &iBuffer);
		glGenBuffers(1, &nBuffer);
		glGenVertexArrays(1, &vaoID);

		vector<Vector3f>& verts = mesh.getVerts();
		vector<Vector3f>& uvs = mesh.getTexCoords();
		vector<Vector3f>& normals = mesh.getNormals();
		vector<Face>& faces = mesh.getFaces();

		int vCount = verts.size();
		int tCount = uvs.size();
		int nCount = normals.size();
		int fCount = faces.size();

		printf("Loaded Model: %s \nV: %d  T: %d  F: %d\n", objLoc, vCount, tCount, fCount);

		/*int vertexCoordCount = vCount*3;
		float* vCoords = new float[vertexCoordCount];
		vector<Vector3f>::iterator itv;
		int ind=0;
		for(itv=verts.begin(); itv<verts.end(); itv++){
			vCoords[ind]=itv->coords[0];
			vCoords[ind+1]=itv->coords[1];
			vCoords[ind+2]=itv->coords[2];
			//printf("Added Vertex: %f %f %f\n", itv->coords[0], itv->coords[1], itv->coords[2]);
			ind+=3;
		}*/

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*vCount, &verts[0].coords[0], GL_STATIC_DRAW);

		//delete[] vCoords;

		/*if(tCount>0){
			glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*tCount, &uvs[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}*/

		/*if(nCount>0){
			glBindBuffer(GL_ARRAY_BUFFER, nBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*nCount, &normals[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}*/
		
		faceCount = fCount*3;
		unsigned int* vIndices = new unsigned int[faceCount];
		Vector3f* nValues = new Vector3f[vCount];
		Vector3f* tValues = new Vector3f[vCount];
		vector<Face>::iterator it;
		int ind=0;
		for(it=faces.begin(); it<faces.end(); it++){
			vIndices[ind]=it->verts.coords[0];
			vIndices[ind+1]=it->verts.coords[1];
			vIndices[ind+2]=it->verts.coords[2];

			nValues[it->verts.coords[0]] = normals[it->normals.coords[0]];
			nValues[it->verts.coords[1]] = normals[it->normals.coords[1]];
			nValues[it->verts.coords[2]] = normals[it->normals.coords[2]];

			tValues[it->verts.coords[0]] = uvs[it->texCoords.coords[0]];
			tValues[it->verts.coords[1]] = uvs[it->texCoords.coords[1]];
			tValues[it->verts.coords[2]] = uvs[it->texCoords.coords[2]];

			ind+=3;
		}

		glBindBuffer(GL_ARRAY_BUFFER, nBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*vCount, nValues, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*vCount, tValues, GL_STATIC_DRAW);

		delete[] tValues;

		//printf("TestDiff: %d %d\n", sizeof(Vector3f)*nCount, sizeof(float)*faceCount);

		delete[] nValues;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*fCount*3, vIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] vIndices;

		glBindVertexArray(vaoID);
		BindVBuffer();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		BindTBuffer();
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		BindNBuffer();
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		BindIBuffer();
		glBindVertexArray(0);
		mState = MODEL_ELEMENTS;
	}

	Model::~Model(){
		if(mState == MODEL_NEW){
			return;
		}
		glDeleteBuffers(1, &vBuffer);
		glDeleteBuffers(1, &tBuffer);
		glDeleteBuffers(1, &nBuffer);
		if(mState == MODEL_ELEMENTS){
			glDeleteBuffers(1, &iBuffer);
		}
		glDeleteVertexArrays(1, &vaoID);
	}

	void Model::BindVBuffer(){
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	}

	void Model::BindTBuffer(){
		glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
	}

	void Model::BindIBuffer(){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
	}

	void Model::BindNBuffer(){
		glBindBuffer(GL_ARRAY_BUFFER, nBuffer);
	}

	unsigned int Model::GetFaceCount(){
		return faceCount;
	}

	void Model::DrawModel(){
		if(mState == MODEL_NEW){
			return;
		}
		glBindVertexArray(vaoID);
		if(mState == MODEL_ELEMENTS){
			glDrawElements(GL_TRIANGLES, GetFaceCount(), GL_UNSIGNED_INT, 0);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, GetFaceCount());
		}
		glBindVertexArray(0);
	}

	Model::Model(){
		mNewData = new ModelData();
		mState = MODEL_NEW;
	}

	void Model::AddVertex(Vec3 Position, Vec2 UV, Vec3 Normal){
		mNewData->vPositions.push_back(Position);
		mNewData->vUVs.push_back(UV);
		mNewData->vNormals.push_back(Normal);
	}

	void Model::Compile(){
		int vCount = mNewData->vPositions.size();
		printf("Compiling Geometry with %i verticies.\n", vCount);
		faceCount = vCount;

		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &tBuffer);
		glGenBuffers(1, &nBuffer);

		glGenVertexArrays(1, &vaoID);

		BindVBuffer();
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*vCount, &(mNewData->vPositions[0].x), GL_STATIC_DRAW);

		BindTBuffer();
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2)*vCount, &(mNewData->vUVs[0].x), GL_STATIC_DRAW);

		BindNBuffer();
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*vCount, &(mNewData->vNormals[0].x), GL_STATIC_DRAW);


		glBindVertexArray(vaoID);

		BindVBuffer();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		BindTBuffer();
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		BindNBuffer();
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glBindVertexArray(0);
		mState = MODEL_ARRAYS;
	}
}