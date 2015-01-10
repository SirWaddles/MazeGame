#ifndef _GRAPHICS_HEADER_
#define _GRAPHICS_HEADER_

//#define DEF_SCREEN_W 800
//#define DEF_SCREEN_H 600

#include <string>
#include <vector>

namespace GraphicsM{

	class Colour{
	public:
		Colour() : r(0.0), g(0.0), b(0.0), a(1.0){};
		Colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255) : r(float(red)/255.0f), g(float(green)/255.0f), b(float(blue)/255.0f), a(float(alpha)/255.0f){};
		float r,g,b,a;
		float GetFR();
		float GetFG();
		float GetFB();
		float GetFA();
	};

	struct Vec3{
		float x;
		float y;
		float z;
		Vec3(float tX, float tY, float tZ) : x(tX), y(tY), z(tZ){}
		Vec3() : x(0.0f), y(0.0f), z(0.0f){}
		friend Vec3 operator+(const Vec3 &a, const Vec3 &b){
			return Vec3(a.x+b.x, a.y+b.y, a.z+b.z);
		}
		friend Vec3 operator-(const Vec3 &a, const Vec3 &b){
			return Vec3(a.x-b.x, a.y-b.y, a.z-b.z);
		}
		friend Vec3 operator*(const Vec3 &a, const Vec3 &b){
			return Vec3(a.x*b.x, a.y*b.y, a.z*b.z);
		}
		friend Vec3 operator*(const Vec3 &a, const float &b){
			return Vec3(a.x*b, a.y*b, a.z*b);
		}
		friend Vec3 operator*(const float &a, const Vec3 &b){
			return b*a;
		}
		float Magnitude();
		void Normalize();
		float Distance(Vec3 &b);
	};

	struct Quaternion{
		float x;
		float y;
		float z;
		float w;
		Quaternion();
		void Normalize();
	};

	struct Vec2{
		float x;
		float y;
		Vec2(float tX, float tY) : x(tX), y(tY){}
		Vec2() : x(0.0f), y(0.0f){}
		friend Vec2 operator+(const Vec2 &a, const Vec2 &b){
			return Vec2(a.x+b.x, a.y+b.y);
		}
		friend Vec2 operator-(const Vec2 &a, const Vec2 &b){
			return Vec2(a.x-b.x, a.y-b.y);
		}
		void Normalize();
		float DotProduct(Vec2 &b);
	};

	enum ShaderType{
		SHADER_VERTEX=0,
		SHADER_GEOMETRY,
		SHADER_FRAGMENT
	};

	class Shader{
	public:
		Shader(ShaderType sType, const std::string &shaderContents);
		void SetName(const char* name);
		const char* GetName();

		unsigned int GetID();

		bool IsShared();
		void SetShared(bool shared);
		~Shader();
	private:
		unsigned int shaderID;
		ShaderType shaderType;
		std::string shaderName;
		bool isShared;
	};

	struct ShaderAttribData {
		std::string name;
		int attribIndex;
		int attribSize;
	};

	class ShaderProgram{
	public:
		ShaderProgram();
		~ShaderProgram();

		void SetName(const char* name);
		const char* GetName();

		unsigned int GetID();

		unsigned int GetUniformID(char* name);

		void AddShader(Shader* pShader);
		void LinkAll();
	private:
		std::vector<Shader*> shaderComponents;
		std::vector<ShaderAttribData> shaderAttribs;
		std::vector<ShaderAttribData> shaderUniforms;
		unsigned int shaderPID;
		std::string programName;
		bool linked;
	};

	struct GlyphData{
		int width;
		int height;
		int tWidth;
		int tHeight;
		int left;
		int top;
		int advance;
		unsigned int texID;
	};

	inline int next_p2 (int a){
		int rval=1;
		while(rval<a) rval<<=1;
		return rval;
	}

	struct VertexD{
		float x,y,z,w;
		float r,g,b,a;
		float u,v;
	};

	struct FontSize{
		int w;
		int h;
	};

	void Set2DVertex(VertexD &oldV, float x, float y, float u, float v);

	class TFont{
	public:
		TFont(const char* fontName, unsigned int h);
		~TFont();

		void RenderText(float x, float y, const char* text, Colour col);
		FontSize MeasureText(const char* text);
	private:
		GlyphData glyphs[128];
		unsigned int vBuffer;
		unsigned int vaoID;
		VertexD drawVerts[6];
	};

	class MTexture {
	public:
		MTexture(const char *path, bool invertY = true);
		~MTexture();

		unsigned int GetTexName();
		Vec2 GetSize();
		
	private:
		unsigned int texLocate;
		unsigned int tW;
		unsigned int tH;

	};

	class Material{
	public:
		Material();
		MTexture* matTexture;
		Colour diffuse;
		Colour specular;
		Colour ambient;
	};

	enum LightType{
		LIGHT_DIRECTIONAL=0,
		LIGHT_OMNI
	};

	class Light{
	public:
		Light();
		LightType lType;
		Vec3 lPos;
		Colour intensity;
		float strength;
		float shiny;
		float wCheck;
	protected:

	};

	enum ModelState {
		MODEL_NEW=0,
		MODEL_ELEMENTS,
		MODEL_ARRAYS
	};
	
	struct ModelData {
		std::vector<Vec3> vPositions;
		std::vector<Vec2> vUVs;
		std::vector<Vec3> vNormals;
	};

	class Model {
	public:
		Model(const char* objLoc);
		Model();
		~Model();

		void BindVBuffer();
		void BindTBuffer();
		void BindIBuffer();
		void BindNBuffer();
		unsigned int GetFaceCount();

		void DrawModel();

		void AddVertex(Vec3 Position, Vec2 UV, Vec3 Normal);
		void Compile();
	private:
		unsigned int vBuffer;
		unsigned int tBuffer;
		unsigned int iBuffer;
		unsigned int nBuffer;
		unsigned int faceCount;
		unsigned int vaoID;

		ModelData* mNewData;
		ModelState mState;
	};

	class ModelInstance{
	public:
		Material* material;
		Model* model;
		Vec3 Position;
		Quaternion Angle;
		Vec3 Scale;
	};

	int ScrH();
	int ScrW();

	void CreateMWindow();
	void DestroyWindow();
	void SwapBuffers();
	void ClearWindow(Colour col);
	void ClearDepth();
	void SetScreenSize(int sW, int sH, bool full);

	void EnableDepth();
	void DisableDepth();
	void SetupDepth();

	void EnableAlpha();
	void DisableAlpha();

	void LoadAllShaders();
	void UseShader(char* programName);
	void UseShader(ShaderProgram* shader);
	ShaderProgram* GetShader(char* programName);
	void NullShader();

	TFont* LoadFont(const char* fontName, unsigned int h);
	void FreeFont(const char* fontName, unsigned int h);
	TFont* GetFont(const char* fontName, unsigned int h);
	FontSize MeasureFont(TFont* font, const char* text);

	void CreateIsometricMatrix();
	void CreatePerspectiveMatrix();
	void PrintModelViewMatrix();
	void LoadMIdentity();
	void UpdateNormalMat();
	float* GetProjectionMatrix();
	float* GetModelViewMatrix();
	float* GetNMatrix();
	void TranslateM(float x, float y, float z);
	void RotateM(float ang, float xa, float ya, float za);
	void ScaleM(float x, float y, float z);
	void RotateM(Quaternion quat);
	void DeRotateM(Quaternion quat);
	Vec3 GetCSpace(Vec3 mSpace);
	Vec3 GetSSpace(Vec3 mSpace);
	float GetLastW();

	void ResetMouseToCenter();

	Vec2 GetFullscreenResolution();
}



#endif //_GRAPHICS_HEADER_