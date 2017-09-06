/**
* @file Main.cpp
*/
#include "GameEngine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <iostream>
using namespace Uniform;

//エンティティの衝突グループID
enum EntityGroupId {
	EntityGroupId_Player,
	EntityGroupId_PlayerShot,
	EntityGroupId_Enemy,
	EntityGroupId_EnemyShot,
	EntityGroupId_Others,
};

//衝突形状リスト
static const Entity::CollisionData collisionDataList[] = {
	{ glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec3(1.0f,1.0f,1.0f) },
	{ glm::vec3(-0.5f,-0.5f,-1.0f),glm::vec3(0.5f,0.5f,1.0f) },
	{ glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec3(1.0f,1.0f,1.0f) },
	{ glm::vec3(-0.25f,-0.25f,-0.25f),glm::vec3(0.25f,0.25f,0.25f) },
};

/**
* @desc 敵の円盤の状態を更新する
*/
struct UpdateToroid {
	void operator()(Entity::Entity& entity, void* ubo, double delta, const glm::mat4& matView, const glm::mat4& matProj) {
		//範囲外に出たら削除する
		const glm::vec3 pos = entity.Position();
		if (std::abs(pos.x) > 40.0f || std::abs(pos.z) > 40.0f) {
			GameEngine::Instance().RemoveEntity(&entity);
			return;
		}

		//円盤を回転させる
		float rot = glm::angle(entity.Rotation());
		rot += glm::radians(180.0f) * static_cast<float>(delta);
		if (rot > glm::pi<float>() * 2.0f) {
			rot -= glm::pi<float>() * 2.0f;
		}
		entity.Rotation(glm::angleAxis(rot, glm::vec3(0, 1, 0)));

		//頂点シェーダのパラメータをUBOにコピーする
		VertexData data;
		data.matModel = entity.TRSMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		memcpy(ubo, &data, sizeof(VertexData));
	}
};

/**
* @desc 自機の弾の更新
*/
struct UpdatePlayerShot {
	void operator()(Entity::Entity& entity, void* ubo, double delta, const glm::mat4& matView, const glm::mat4& matProj) {
		const glm::vec3 pos = entity.Position();
		if (std::abs(pos.x) > 40 || pos.z < -4 || pos.z > 40) {
			entity.Destroy();
			return;
		}
		//頂点シェーダーのパラメータをUBOにコピーする
		VertexData data;
		data.matModel = entity.TRSMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		memcpy(ubo, &data, sizeof(VertexData));

	}
};
/**
* @desc 自機の更新
*/
struct UpdatePlayer {
	void operator()(Entity::Entity& entity, void* ubo, double delta, const glm::mat4& matView, const glm::mat4& matProj) {
		GameEngine& game = GameEngine::Instance();
		const GamePad gamepad = game.GetGamePad();
		glm::vec3 vec;
		float rotZ = 0;
		if (gamepad.buttons & GamePad::DPAD_LEFT) {
			vec.x = 1;
			rotZ = -glm::radians(30.0f);
		}
		else if (gamepad.buttons & GamePad::DPAD_RIGHT) {
			vec.x = -1;
			rotZ = glm::radians(30.0f);
		}
		if (gamepad.buttons & GamePad::DPAD_UP) {
			vec.z = 1;
		}
		else if (gamepad.buttons & GamePad::DPAD_DOWN) {
			vec.z = -1;
		}
		if (vec.x || vec.z) {
			vec = glm::normalize(vec) * 15.0f;
		}
		entity.Velocity(vec);
		entity.Rotation(glm::quat(glm::vec3(0, 0, rotZ)));
		glm::vec3 pos = entity.Position();
		pos = glm::min(glm::vec3(11, 100, 20), glm::max(pos, glm::vec3(-11, -100, 1)));
		entity.Position(pos);

		if (gamepad.buttonDown & GamePad::A) {
			shotInterval -= delta;
			if (shotInterval <= 0) {
				glm::vec3 pos = entity.Position();
				pos.x -= 0.3f;
				for (int i = 0; i < 2; ++i) {
					if (Entity::Entity* p = game.addEntity(EntityGroupId_PlayerShot, pos,
						"NormalShot", "Res/Player.bmp", UpdatePlayerShot())) {
						p->Velocity(glm::vec3(0, 0, 80));
						p->Collision(collisionDataList[EntityGroupId_PlayerShot]);
					}
					pos.x += 0.6f;
				}
				shotInterval = 0.25;
			}
		}
		else {
			shotInterval = 0;
		}

		VertexData data;
		data.matModel = entity.TRSMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		memcpy(ubo, &data, sizeof(VertexData));
	}

private:
	double shotInterval = 0;


};
/**
* @desc 爆発の更新
*/
struct UpdateBlast {
	void operator()(Entity::Entity& entity, void* ubo, double delta,
		const glm::mat4& matView, const glm::mat4& matProj) {
		timer += delta;
		if (timer >= 0.5) {
			entity.Destroy();
			return;
		}
		//変化量
		const float variation = static_cast<float>(timer * 4);
		//徐々に拡大する
		entity.Scale(glm::vec3(static_cast<float>(1 + variation)));
		//時間経過で色と透明度を変化させる
		static const glm::vec4 color[] = {
			glm::vec4(1.0f,1.0f,0.75f,1),
			glm::vec4(1.0f,0.5f,0.1f,1),
			glm::vec4(0.25f,0.1f,0.1f,0),
		};
		const glm::vec4 col0 = color[static_cast<int>(variation)];
		const glm::vec4 col1 = color[static_cast<int>(variation) + 1];
		//const glm::vec4 newColor = glm::mix(col0, col1, std::fmod(variation + 1));
		//entity.Color(newColor);
		//Y軸回転させる
		glm::vec3 euler = glm::eulerAngles(entity.Rotation());
		euler.y += glm::radians(60.0f) * static_cast<float>(delta);
		entity.Rotation(glm::quat(euler));
		//頂点シェーダのパラメータをUBOにコピーする
		VertexData data;
		data.matModel = entity.TRSMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		memcpy(ubo, &data, sizeof(VertexData));
	}
	double timer = 0;
};




/**
* @desc ゲーム状態の更新
*/
struct Update {
	void operator()(double delta) {
		GameEngine& game = GameEngine::Instance();

		if (!pPlayer) {
			pPlayer = game.addEntity(EntityGroupId_Player, glm::vec3(0, 0, 2),
				"Aircraft", "Res/Player.bmp", UpdatePlayer());
			pPlayer->Collision(collisionDataList[EntityGroupId_Player]);
		}
		game.Camera({ glm::vec4(0,20,-8,1),glm::vec3(0,0,12),glm::vec3(0,0,1) });
		game.AmbientLight(glm::vec4(0.05f,0.1f,0.2f,1));
		game.Light(0, {glm::vec4(40,100,10,1),glm::vec4(12000,12000,12000,1)});
		std::uniform_int_distribution<> distributerX(-12, 12);
		std::uniform_int_distribution<> distributerZ(40,44);
		interval -= delta;
		if (interval <= 0) {
			const std::uniform_real_distribution<> rndInterval(8.0, 16.0);
			const std::uniform_int_distribution<> rndAddingCount(1, 5);
			const std::uniform_real_distribution<> rndPopintTime(2.0, 5.0);
			for (int i = rndAddingCount(game.Rand()); i > 0; --i) {
				const glm::vec3 pos(distributerX(game.Rand()), 0, distributerZ(game.Rand()));
				if (Entity::Entity* p = game.addEntity(EntityGroupId_Enemy,pos,"Toroid","Res/Toroid.bmp",UpdateToroid())) {
					p->Velocity(glm::vec3(pos.x < 0 ? 4.0f : -4.0f, 0, -16.0f));
					p->Collision(collisionDataList[EntityGroupId_Enemy]);
				}
			}
			interval = rndInterval(game.Rand());
		}

		char str[16];
		snprintf(str, 16, "%08d", game.Socre());
		game.FontScale(glm::vec2(2));
		game.FontColor(glm::vec4(1));
		game.AddString(glm::vec2(-0.2f, 0.9f), str);

	}
	double interval = 0;
	Entity::Entity* pPlayer = nullptr;
};

/**
* @desc 自機の弾と敵の衝突処理
*/
void PlayerShotAndEnemyCollisionHandler(Entity::Entity & lhs, Entity::Entity& rhs) {
	GameEngine& game = GameEngine::Instance();
	if (Entity::Entity* p = game.addEntity(EntityGroupId_Others, rhs.Position(),
		"Blast", "Res/Toroid.bmp", UpdateBlast())) {
		const std::uniform_real_distribution <float> rotRange(0.0f, glm::pi<float>() * 2);
		p->Rotation(glm::quat(glm::vec3(0, rotRange(game.Rand()), 0)));
		game.Score(game.Socre() + 100);
	}
	lhs.Destroy();
	rhs.Destroy();
}

/**
* GLFWからのエラー報告を処理する
*
* @param error エラー番号
* @param desc エラーの内容
*/
void ErrorCallBack(int error, const char* desc) {
	std::cerr << "ERROR:" << desc << std::endl;
}

/**
* @desc メイン処理
*/
int main() {
	GameEngine& game = GameEngine::Instance();
	if (!game.Init(800, 600, "OpenGL Tutorial")) {
		return 1;
	}
	game.LoadTextureFromFile("Res/Toroid.bmp");
	game.LoadTextureFromFile("Res/Player.bmp");
	game.LoadMeshFromFile("Res/Toroid.fbx");
	game.LoadMeshFromFile("Res/Player.fbx");
	game.LoadMeshFromFile("Res/Blast.fbx");

	game.LoadFontFromFile("Res/font/UniNeue.fnt");

	game.CollisionHandler(EntityGroupId_PlayerShot, EntityGroupId_Enemy, &PlayerShotAndEnemyCollisionHandler);
	game.UpdateFunc(Update());
	game.Run();
	return 0;
}