#include "graphics/particles.h"
#include "delta.h"
#include <GL/glew.h>
#include <SDL.h>

using namespace GraphicsM;
using namespace GParticles;
using std::list;

void GParticles::TParticle::SetPosition(Vec3 nPos){
	pPosition = nPos;
}

void GParticles::TParticle::SetColour(Colour nColour){
	pColour = nColour;
}

void GParticles::TParticle::SetDirection(Vec3 nDir){
	pDirection = nDir;
}

void GParticles::TParticle::SetLife(unsigned int nLife){
	pLife = nLife;
}

void GParticles::TParticle::SetSize(float startSize, float endSize){
	pSize=startSize;
	sizeLost = ((startSize-endSize)*1000.0f)/float(pLife);
}

bool GParticles::TParticle::Think(unsigned int nTime, float fTime){
	if(nTime > pCreateTime+pLife) return false;

	pPosition = pPosition + (pDirection*fTime);
	pSize = pSize - (sizeLost*fTime);
	return true;
}

Colour GParticles::TParticle::GetColour(){
	 return pColour;
}

Vec3 GParticles::TParticle::GetPosition(){
	return pPosition;
}

float GParticles::TParticle::GetSize(){
	return pSize;
}

GParticles::TParticle::TParticle(){
	pCreateTime = SDL_GetTicks();
	pSize = 0.01f;
}

struct CompiledParticle{
	Vec3 pPos;
	Colour pCol;
	float pSize;
};

GParticles::TParticleEmitter::TParticleEmitter(){

	ShaderProgram* nShader = GraphicsM::GetShader("particle");
	baseImage = nShader->GetUniformID("baseImage");
	projMat = nShader->GetUniformID("projMat");
	modelViewMat = nShader->GetUniformID("modelView");

	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &pBuffer);

	CompiledParticle nullParts[MAX_PARTICLES];
	glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CompiledParticle)*MAX_PARTICLES, &nullParts[0], GL_STREAM_DRAW);

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CompiledParticle), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CompiledParticle), (void*)sizeof(Vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(CompiledParticle), (void*)(sizeof(Vec3)+sizeof(Colour)));
}

GParticles::TParticleEmitter::~TParticleEmitter(){
	list<TParticle*>::iterator it;
	for(it=particleList.begin(); it!=particleList.end(); it++){
		delete (*it);
	}
	glDeleteBuffers(1, &pBuffer);
	glDeleteVertexArrays(1, &vaoID);
}

void GParticles::TParticleEmitter::AddParticle(TParticle* nPart){
	if(!nPart) return;
	particleList.push_back(nPart);
}

void GParticles::TParticleEmitter::SetParticleTexture(MTexture* nTex){
	pTexture = nTex;
}

void GParticles::TParticleEmitter::Think(){
	// Direction and fades are per-second.
	unsigned int nTime = SDL_GetTicks();
	float fTime = FrameTime();

	std::list<TParticle*>::iterator it = particleList.begin();
	std::list<std::list<TParticle*>::iterator> toBeRemoved;
	for(it=particleList.begin(); it!=particleList.end(); it++){
		if(!(*it)->Think(nTime, fTime)){
			toBeRemoved.push_back(it);
		}
	}
	
	std::list<std::list<TParticle*>::iterator>::iterator itRem;
	for(itRem = toBeRemoved.begin(); itRem!=toBeRemoved.end(); itRem++){
		delete (**itRem);
		particleList.erase(*itRem);
	}
}

void GParticles::TParticleEmitter::DrawParticles(){

	int pCount = particleList.size();
	//printf("Particles: %i\n", pCount);
	if(pCount<=0){
		return;
	}

	ShaderProgram* nShader = GraphicsM::GetShader("particle");
	GraphicsM::UseShader(nShader);

	int curPart = 0;
	CompiledParticle* bufParts = new CompiledParticle[pCount];

	list<TParticle*>::iterator it;
	for(it=particleList.begin(); it!= particleList.end(); it++){
		bufParts[curPart].pCol = (*it)->GetColour();
		bufParts[curPart].pPos = (*it)->GetPosition();
		bufParts[curPart].pSize = (*it)->GetSize();
		curPart++;
	}
	EnableDepth();
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	

	glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(CompiledParticle)*curPart, &bufParts[0]);

	delete[] bufParts;

	glUniformMatrix4fv(projMat, 1, false, GetProjectionMatrix());
	glUniformMatrix4fv(modelViewMat, 1, false, GetModelViewMatrix());

	glUniform1i(baseImage, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pTexture->GetTexName());
	glEnable(GL_TEXTURE_2D);

	glBindVertexArray(vaoID);
	glDrawArrays(GL_POINTS, 0, pCount);

	glDepthMask(GL_TRUE);
}