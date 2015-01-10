#ifndef _PARTICLES_HEADER_
#define _PARTICLES_HEADER_

#include "graphics/graphics.h"
#include <list>

#define MAX_PARTICLES 2000

namespace GParticles {

	class TParticle{
	public:
		void SetPosition(GraphicsM::Vec3 nPos);
		void SetColour(GraphicsM::Colour nColour);
		void SetDirection(GraphicsM::Vec3 nDir);
		void SetLife(unsigned int nLife);
		void SetSize(float startSize, float endSize);

		GraphicsM::Vec3 GetPosition();
		GraphicsM::Colour GetColour();
		float GetSize();

		bool Think(unsigned int nTime, float fTime);
		TParticle();
	protected:
		GraphicsM::Vec3 pPosition;
		GraphicsM::Colour pColour;
		float pSize;
		
		GraphicsM::Vec3 pDirection;
		unsigned int pLife;
		unsigned int pCreateTime;
		float sizeLost;
	private:

	};

	class TParticleEmitter {
	public:
		TParticleEmitter();
		~TParticleEmitter();

		void SetParticleTexture(GraphicsM::MTexture* nTex);
		void AddParticle(TParticle* nPart);
		void DrawParticles();
		void Think();
	protected:
		std::list<TParticle*> particleList;

		unsigned int vaoID;
		unsigned int pBuffer;
		GraphicsM::MTexture* pTexture;

		unsigned int baseImage;
		unsigned int modelViewMat;
		unsigned int projMat;
	private:

	};

}





#endif