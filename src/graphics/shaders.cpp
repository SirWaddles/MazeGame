#include "graphics/graphics.h"

#include <GL/glew.h>
#include <SDL.h>

#include <map>
#include "util/files.h"
#include <fstream>

using namespace std;

namespace GraphicsM{
	Shader::Shader(ShaderType sType, const string &shaderContents){
		isShared=false;
		GLenum gShaderType;
		switch(sType){
		case SHADER_VERTEX:
			gShaderType=GL_VERTEX_SHADER;
			break;
		case SHADER_GEOMETRY:
			gShaderType=GL_GEOMETRY_SHADER;
			break;
		case SHADER_FRAGMENT:
			gShaderType=GL_FRAGMENT_SHADER;
			break;
		}
		shaderID=glCreateShader(gShaderType);
		const char *strShader = shaderContents.c_str();
		glShaderSource(shaderID, 1, &strShader, NULL);

		glCompileShader(shaderID);
		int status;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
		if(status==GL_FALSE){
			int logLength;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

			char* strLog = new char[logLength+2];
			glGetShaderInfoLog(shaderID, logLength, NULL, strLog);
			printf("Compile failed: %s\n", strLog);
			delete[] strLog;
		}
	}

	Shader::~Shader(){
		glDeleteShader(shaderID);
	}

	unsigned int Shader::GetID(){
		return shaderID;
	}

	void Shader::SetName(const char* name){
		shaderName=name;
	}

	const char* Shader::GetName(){
		return shaderName.c_str();
	}

	bool Shader::IsShared(){
		return isShared;
	}

	void Shader::SetShared(bool shared){
		isShared=shared;
	}

	ShaderProgram::ShaderProgram(){
		shaderPID = glCreateProgram();
		linked=false;
	}

	ShaderProgram::~ShaderProgram(){
		glDeleteProgram(shaderPID);
	}

	void ShaderProgram::LinkAll(){
		linked=true;
		glLinkProgram(shaderPID);
		int status;
		glGetProgramiv(shaderPID, GL_LINK_STATUS, &status);
		if(status==GL_FALSE){
			int logLength;
			glGetProgramiv(shaderPID, GL_INFO_LOG_LENGTH, &logLength);

			char* strLog = new char[logLength+2];
			glGetProgramInfoLog(shaderPID, logLength, NULL, strLog);
			printf("Linker failed: %s\n", strLog);

		}

		vector<Shader*>::iterator it;
		for(it=shaderComponents.begin(); it<shaderComponents.end(); it++){
			glDetachShader(shaderPID, (*it)->GetID());
			if(!(*it)->IsShared()){
				delete (*it);
			}
		}
		shaderComponents.clear();

		int attribs;
		glGetProgramiv(shaderPID, GL_ACTIVE_ATTRIBUTES, &attribs);
		for(int i=0; i<attribs; i++){
			char* name = new char[50];
			GLenum dataType;
			int sizeAttrib;
			glGetActiveAttrib(shaderPID, i, 49, NULL, &sizeAttrib, &dataType, name);
			ShaderAttribData data;
			data.attribIndex=glGetAttribLocation(shaderPID, name);
			data.attribSize=sizeAttrib;
			data.name=name;
			shaderAttribs.push_back(data);
			printf("Component Name: %s Index: %d\n", name, data.attribIndex);
			delete[] name;
		}
		int uniforms;
		glGetProgramiv(shaderPID, GL_ACTIVE_UNIFORMS, &uniforms);
		for(int i=0; i<uniforms; i++){
			char* name = new char[50];
			GLenum dataType;
			int sizeAttrib;
			glGetActiveUniform(shaderPID, i, 49, NULL, &sizeAttrib, &dataType, name);
			ShaderAttribData data;
			data.attribIndex=glGetUniformLocation(shaderPID, name);
			data.attribSize=sizeAttrib;
			data.name=name;
			shaderUniforms.push_back(data);
			printf("Uniform Name: %s Index %d\n", name, data.attribIndex);
			delete[] name;
		}
	}

	void ShaderProgram::AddShader(Shader* pShader){
		if(linked){
			return;
		}

		glAttachShader(shaderPID, pShader->GetID());
		shaderComponents.push_back(pShader);
	}

	void ShaderProgram::SetName(const char* name){
		programName=name;
	}

	const char* ShaderProgram::GetName(){
		return programName.c_str();
	}

	unsigned int ShaderProgram::GetID(){
		return shaderPID;
	}

	unsigned int ShaderProgram::GetUniformID(char* name){
		vector<ShaderAttribData>::iterator it;
		for(it=shaderUniforms.begin(); it<shaderUniforms.end(); it++){
			if(it->name.compare(name)==0){
				return it->attribIndex;
			}
		}
		return -1;
	}

	struct FileData{
		string location;
		string extension;
		string name;
	};

	FileData GetFileData(string dir, string name){
		FileData finData;
		unsigned int foundExt;
		foundExt=name.find_last_of('.');
		finData.extension=name.substr(foundExt+1);
		unsigned int foundName;
		foundName=name.find_last_of("/\\");
		if(foundName==string::npos){
			foundName=0;
		}
		finData.name=name.substr(foundName, foundExt);

		finData.location=dir + name;
		return finData;
	}

	map<string, ShaderProgram*> gSPrograms;

	void LoadAllShaders(){
		map<string, Shader*> gShaders;
		vector<string> files=Util::GetDirectoryContents("shaders/");
		vector<string>::iterator it;
		for(it=files.begin(); it<files.end(); it++){
			FileData fData=GetFileData("shaders/", (*it));
			if(fData.extension!="vert" && fData.extension!="frag" && fData.extension!="geom"){
				continue;
			}

			Shader* newShader=NULL;
			if(fData.extension=="vert"){
				newShader = new Shader(SHADER_VERTEX, Util::LoadFile(fData.location.c_str()));
			}
			if(fData.extension=="frag"){
				newShader = new Shader(SHADER_FRAGMENT, Util::LoadFile(fData.location.c_str()));
			}
			if(fData.extension=="geom"){
				newShader = new Shader(SHADER_GEOMETRY, Util::LoadFile(fData.location.c_str()));
			}
			newShader->SetName(fData.name.c_str());
			newShader->SetShared(true);
			string fullLoc=(*it);
			gShaders[fullLoc]=newShader;
		}

		for(it=files.begin(); it<files.end(); it++){
			FileData fData=GetFileData("shaders/", (*it));
			if(fData.extension=="sha"){
				ifstream shaderData(fData.location);
				string sLine;
				ShaderProgram* newProgram = new ShaderProgram();
				while(shaderData.good()){
					getline(shaderData, sLine);
					newProgram->AddShader(gShaders[sLine]);
				}
				newProgram->SetName(fData.name.c_str());
				newProgram->LinkAll();
				gSPrograms[fData.name]=newProgram;
				printf("Program made: %s\n", fData.name.c_str());
			}
		}
		map<string, Shader*>::iterator git;
		for(git=gShaders.begin(); git!=gShaders.end(); git++){
			delete git->second;
		}
	}

	void UseShader(char* programName){
		map<string, ShaderProgram*>::iterator it = gSPrograms.find(programName);
		if(it!=gSPrograms.end()){
			glUseProgram(it->second->GetID());
		} else {
			printf("Shader: %s not found.\n", programName);
		}
	}

	void UseShader(ShaderProgram* shader){
		glUseProgram(shader->GetID());
	}

	ShaderProgram* GetShader(char* programName){
		map<string, ShaderProgram*>::iterator it = gSPrograms.find(programName);
		if(it!=gSPrograms.end()){
			return it->second;
		}
		return NULL;
	}

	void NullShader(){
		glUseProgram(0);
	}
}