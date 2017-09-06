#pragma once

#include "UniformBuffer.h"
#include "OffscreenBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Entity.h"
#include "Uniform.h"
#include "GamePad.h"
#include "Font.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <functional>
#include <random>

/**
* @desc ゲームエンジンクラス
*/
class GameEngine {
public:
	//ゲーム状態を更新する関数の型
	typedef std::function<void(double)>UpdateFuncType;
	//カメラデータ
	struct CameraData {
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up;
	};

	static GameEngine& Instance();
	bool Init(int w, int g, const char* title);
	void Run();
	void UpdateFunc(const UpdateFuncType& func);
	const UpdateFuncType& UpdateFunc()const;

	//================================================
	//ここからメンバ関数を追加する
	//================================================

	bool LoadTextureFromFile(const char* filename);
	bool LoadMeshFromFile(const char* filename);
	Entity::Entity* addEntity(int groupId,const glm::vec3& pos, const char* meshName,
		const char* texName, Entity::Entity::UpdateFuncType func,bool hasLight = true);
	void RemoveEntity(Entity::Entity*);
	void Light(int index, const Uniform::PointLight& light);
	const Uniform::PointLight& Light(int index)const;
	void AmbientLight(const glm::vec4& color);
	const glm::vec4& AmbientLight()const;
	void Camera(const CameraData& cam);
	const CameraData& Camera()const;
	std::mt19937& Rand();
	const GamePad& GetGamePad()const;
	void CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler);
	const Entity::CollisionHandlerType& CollisionHandler(int gind0, int gid1)const;
	void ClearCollisionHandlerList();

	bool LoadFontFromFile(const char* filename) {
		return fontRenderer.LoadFromFile(filename);
	}
	bool AddString(const glm::vec2& pos, const char* str) {
		return fontRenderer.AddString(pos, str);
	}
	void FontScale(const glm::vec2& scale) { fontRenderer.Scale(scale); }
	void FontColor(const glm::vec4& color) { fontRenderer.Color(color); }

	void Score(int s) { score = s; }
	int Socre()const { return score; }

	const TexturePtr& GetTexture(const char* fileName)const {
		return textureBuffer.find(fileName)->second;
	}

	//================================================
	//ここまで
	//================================================

private:
	GameEngine() = default;
	~GameEngine();
	GameEngine(const GameEngine&) = delete;
	GameEngine& operator=(const GameEngine&) = delete;
	void Update(double delta);
	void Render()const;

private:
	bool isInitialized = false;
	UpdateFuncType updateFunc;

private:
	//================================================
	//ここからメンバ変数を追加する
	//================================================
	GLuint vbo = 0;
	GLuint ibo = 0;
	GLuint vao = 0;
	UniformBufferPtr uboLight;
	UniformBufferPtr uboPostEffect;
	Shader::ProgramPtr progTutorial;
	Shader::ProgramPtr progColorFilter;
	OffscreenBufferPtr offscreen;

	std::unordered_map<std::string, TexturePtr> textureBuffer;
	Mesh::BufferPtr meshBuffer;
	Entity::BufferPtr entityBuffer;
	Font::Renderer fontRenderer;
	Uniform::LightData lightData;
	CameraData camera;
	std::mt19937 rand;
	//スコア
	int score = 0
		;
	//================================================
	//ここまで
	//================================================

};