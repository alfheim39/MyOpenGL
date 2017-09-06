#include "GameEngine.h"
#include "GLFWEW.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>


//頂点データ型
struct Vertex {
	glm::vec3 position;//座標
	glm::vec4 color;//色
	glm::vec2 texCoord;//テクスチャ座標
};

//頂点データ
const Vertex vertices[] = {

	{ { -0.5f,-0.3f,0.5f },{ 0.0f,0.0f,1.0f,1.0f },{ 0.0f,0.0f } },
	{ { 0.3f,-0.3f,0.5f },{ 0.0f,1.0f,0.0f,1.0f },{ 1.0f,0.0f } },
	{ { 0.3f, 0.5f,0.5f },{ 0.0f,0.0f,1.0f,1.0f },{ 1.0f,1.0f } },
	{ { -0.5f, 0.5f,0.5f },{ 1.0f,0.0f,0.0f,1.0f },{ 0.0f,1.0f } },

	{ { -0.3f, 0.3f,0.1f },{ 0.0f,0.0f,1.0f,1.0f },{ 0.0f,1.0f } },
	{ { -0.3f,-0.5f,0.1f },{ 0.0f,1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
	{ { 0.5f,-0.5f,0.1f },{ 0.0f,0.0f,1.0f,1.0f },{ 1.0f,0.0f } },
	{ { 0.5f,-0.5f,0.1f },{ 1.0f,0.0f,0.0f,1.0f },{ 1.0f,0.0f } },
	{ { 0.5f, 0.3f,0.1f },{ 1.0f,1.0f,0.0f,1.0f },{ 1.0f,1.0f } },
	{ { -0.3f, 0.3f,0.1f },{ 1.0f,0.0f,0.0f,1.0f },{ 0.0f,1.0f } },

	{ { -1.0f,-1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
	{ { 1.0f,-1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
	{ { 1.0f, 1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
	{ { -1.0f, 1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 1.0f,1.0f } },

};

//インデックスデータ
const GLuint indeices[] = {
	0,1,2,2,3,0,
	4,5,6,7,8,9,
	10,11,12,12,13,10
};

/**
* @desc 部分描画データ
*/
struct RenderingPart {
	GLsizei size;///描画するインデックス数
	GLvoid* offset;///描画開始インデックスのバイトオフセット
};

/**
* @desc RenderingPartを作成する
* @param size	描画するインデックス数
* @param offset	描画開始インデックスのオフセット（インデックス単位）
* @return 作成した部分描画オブジェクト
*/
constexpr RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return{ size,reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* @desc 部分描画データリスト
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12,0),
	MakeRenderingPart(6, 12),
};





/**
* Vertex Buffer Object を作成する
*
* @param size 頂点データのサイズ
* @param data 頂点データへのポインタ
* @return 作成したVBO
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid* data) {
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}

/**
* Index Buffer Objectを作成する
*
* @param size インデックスデータのサイズ
* @param date インデックスデータへのポインタ
*
* @return 作成したIBO
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data) {
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}


/**
* 頂点アトリビュートを設定する
*
* @param index	頂点アトリビュートのインデックス
* @param cls	頂点データ型名
* @param mbr	頂点アトリビュートに設定するclsのメンバ変数名
*/
#define SetVertexAttribPointer(index,cls,mbr)SetVertexAttribPointerI(\
	index,\
	sizeof(cls::mbr)/sizeof(float),\
	sizeof(cls),\
	reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

void SetVertexAttribPointerI(
	GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}
/**
* @desc UniformBlockObjectを作成する
* @param size Uniform Block のサイズ
* @param data Uniform Blockに転送するデータへのポインタ
* @return 作成したUBO
*/
GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr) {
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return ubo;
}

/**
* Vertex Array Object を作成する
*
* @param vbo VAOに関連付けられるVBO
* @param ibo VAOに関連付けられるIBO
*
* @return 作成したVAO
*/
GLuint CreateVAO(GLuint vbo, GLuint ibo) {
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	return vao;
}






/**
* @desc ゲームエンジンのインスタンスを取得する
* @return ゲームエンジンのインスタンス
*/
GameEngine& GameEngine::Instance() {
	static GameEngine instance;
	return instance;
}

/**
* @desc ゲームエンジンを初期化する
* @param ウィンドウの描画範囲の幅（ピクセル）
* @param ウィンドウの描画範囲の高さ（ピクセル）
* @param ウィンドウタイトル(UTF-8の０終端文字列
* @return 初期化成功の成否
*/
bool GameEngine::Init(int w, int h, const char* title) {
	if (isInitialized) {
		return true;
	}
	if (!GLFWEW::Window::Instance().Init(w, h, title)) {
		return false;
	}

	//================================================
	//ここから初期化処理を追加する
	//================================================
	vbo = CreateVBO(sizeof(vertices), vertices);
	ibo = CreateIBO(sizeof(indeices), indeices);
	vao = CreateVAO(vbo, ibo);
	uboLight = UniformBuffer::Create(sizeof(Uniform::LightData), 1, "LightData");
	uboPostEffect = UniformBuffer::Create(
		sizeof(Uniform::PostEffectData),2,"PostEffectData");
	progTutorial = Shader::Program::Create("Res/Tutorial.vert", "Res/Tutorial.frag");
	progColorFilter = Shader::Program::Create("Res/ColorFilter.vert","Res/ColorFilter.frag");
	offscreen = OffscreenBuffer::Create(800, 600);
	if (!vbo || !ibo || !vao || !uboLight || !uboPostEffect || !progTutorial || !progColorFilter || !offscreen) {
		std::cerr << "ERROR:GameEngineの初期化に失敗" << std::endl;
		return false;
	}
	progTutorial->UniformBlockBinding("VertexData", 0);
	progTutorial->UniformBlockBinding("LightData", 1);
	progColorFilter->UniformBlockBinding("PostEffectData", 2);

	meshBuffer = Mesh::Buffer::Create(10 * 1024, 30 * 1024);
	if (!meshBuffer) {
		std::cerr << "ERROR:GameEngineの初期化に失敗" << std::endl;
		return false;
	}
	entityBuffer = Entity::Buffer::Create(
		1024,sizeof(Uniform::VertexData),0,"VertexData");
	if (!entityBuffer) {
		std::cerr << "ERROR:GameEngineの初期化に失敗" << std::endl;
		return false;
	}
	rand.seed(std::random_device()());
	
	fontRenderer.Init(1024, glm::vec2(800, 600));
	
	
	//================================================
	//ここまで
	//================================================

	isInitialized = true;
	return true;
}

/**
* @desc ゲームを実行する
*/
void GameEngine::Run() {
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	double prevTime = glfwGetTime();
	while (!window.ShouldClose()) {
		const double curTime = glfwGetTime();
		const double delta = curTime - prevTime;
		prevTime = curTime;
		window.UpdateGamePad();
		Update(glm::min(0.25, delta));
		Render();
		window.SwapBuffers();

	}
}

/**
* @desc 状態更新関数を設定する
* @param 設定する更新関数
*/
void GameEngine::UpdateFunc(const UpdateFuncType& func) {
	updateFunc = func;
}

/**
* @desc テクスチャを読み込む
* @param テクスチャファイル名
* @return 読み込みの成否
*/
bool GameEngine::LoadTextureFromFile(const char* filename) {
	const auto itr = textureBuffer.find(filename);
	if (itr != textureBuffer.end()) {
		return true;
	}
	TexturePtr texture = Texture::LoadFromFile(filename);
	if (!texture) {
		return false;
	}
	textureBuffer.insert(std::make_pair(std::string(filename), texture));
	return true;
}

/**
* @desc メッシュを読み込む
* @param メッシュファイル名
* @return 読み込みの成否
*/
bool GameEngine::LoadMeshFromFile(const char* filename) {
	return meshBuffer->LoadMeshFromFile(filename);
}

/**
* @desc エンティティを追加する
* @param エンティティのグループID
* @param エンティティの座標
* @param エンティティの表示に使用するメッシュ名
* @param エンティティの表示に使うテクスチャファイル名
* @param エンティティの状態を更新する関数（または関数オブジェクト）
* @return 追加したエンティティへのポインタ
* @tips	これ以上エンティティを追加できない場合はnullptrが返される
*		回転や拡大率はこのポインタ経由で設定する
*		なお、このポインタをアプリケーション側で保持する必要はない
*/
Entity::Entity* GameEngine::addEntity(int groupId,const glm::vec3& pos, const char* meshName, const char* texName,
									Entity::Entity::UpdateFuncType func,bool hasLight ) {
	const Mesh::MeshPtr& mesh = meshBuffer->GetMesh(meshName);
	const TexturePtr& tex = textureBuffer.find(texName)->second;
	return entityBuffer->AddEntity(groupId,pos, mesh, tex, progTutorial, func);
}

/**
* @desc エンティティを削除する
* @param 削除するエンティティのポインタ
*/
void GameEngine::RemoveEntity(Entity::Entity* e) {
	entityBuffer->RemoveEntity(e);
}

/**
* @desc ライトを設定する 
* @param 設定するライトのインデックス
* @param ライトデータ
*/
void GameEngine::Light(int index, const Uniform::PointLight& light) {
	if (index < 0 || index >= Uniform::maxLightCount) {
		std::cerr << "WARNING:'" << index << "'は不正なライトインデックスです" << std::endl;
		return;
	}
	lightData.light[index] = light;
}

/**
* @desc ライトを取得する
* @param 取得するライトのインデックス
* @return ライトデータ
*/
const Uniform::PointLight& GameEngine::Light(int index)const {
	if (index < 0 || index >= Uniform::maxLightCount) {
		std::cerr << "WARNING:'" << index << "'は不正なライトインデックスです" << std::endl;
		static const Uniform::PointLight dummy;
		return dummy;
	}
	return lightData.light[index];
}

/**
* @desc 環境光を設定する
* @param 環境光の明るさ
*/
void GameEngine::AmbientLight(const glm::vec4& color) {
	lightData.ambientColor = color;
}

/**
* @desc 環境光を取得する
* @return 環境光の明るさ
*/
const glm::vec4& GameEngine::AmbientLight()const {
	return lightData.ambientColor;
}

/**
* @desc 視点の位置と姿勢を設定する
* @param cam設定するカメラデータ
*/
void GameEngine::Camera(const CameraData& cam) {
	camera = cam;
}

/**
* @desc 視点の位置と姿勢を取得する
* @return カメラデータ
*/
const GameEngine::CameraData& GameEngine::Camera()const {
	return camera;
}

/**
* @desc 乱数オブジェクトを取得する
* @return 乱数オブジェクト
*/
std::mt19937& GameEngine::Rand() {
	return rand;
}

/**
* @desc ゲームパッドの状態を取得する
*/
const GamePad& GameEngine::GetGamePad()const {
	return GLFWEW::Window::Instance().GetGamePad();
}

/**
* @desc 衝突解決ハンドラを設定する
* @param 衝突対象のグループID
* @param 衝突対象のグループID
* @param 衝突解決ハンドラ
* @tips	衝突が発生し衝突解決ハンドラが呼び出される時、より小さいグループIDを持つエンティティから先に渡される
*		ここで指定したグループIDの順序とは無関係であることに注意すること
* ex)
*		CollisionHandler(10,1,Func)
*		というコードでハンドラを登録したとする.衝突が発生すると
*		Func(グループID=1のエンティティ,グループID=10のエンティティ)
*		のように呼び出される
*/
void GameEngine::CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler) {
	entityBuffer->CollisionHandler(gid0, gid1, handler);
}

/**
* @desc 衝突解決ハンドラを取得する
* @param 衝突対象のグループID０
* @param 衝突対象のグループID１
* @return 衝突解決ハンドラ
*/
const Entity::CollisionHandlerType& GameEngine::CollisionHandler(int gid0, int gid1)const {
	return entityBuffer->CollisionHandler(gid0, gid1);
}

/**
* @desc 衝突解決ハンドラのリストをクリアする
*/
void GameEngine::ClearCollisionHandlerList() {
	entityBuffer->ClearCollisionHandlerList();
}


/**
* @desc 状態更新関数を取得する
* @return 設定されている更新関数
*/
const GameEngine::UpdateFuncType& GameEngine::UpdateFunc()const {
	return updateFunc;
}

/**
* @desc デストラクタ
*/
GameEngine::~GameEngine() {
	//ここに終了処理を追加する
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (ibo) {
		glDeleteBuffers(1, &ibo);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}

}

/**
* @desc ゲームの状態を更新する
* @param 前回の更新からの経過時間（秒）
*/
void GameEngine::Update(double delta) {
	fontRenderer.MapBuffer();
	if (updateFunc) {
		updateFunc(delta);
	}
	//================================================
	//ここから更新処理を追加する
	//================================================
	const glm::mat4x4 matProj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 200.0f);
	const glm::mat4x4 matView = glm::lookAt(camera.position, camera.target, camera.up);
	entityBuffer->Update(delta, matView, matProj);
	fontRenderer.UnmapBuffer();
	//================================================
	//ここまで
	//================================================
}

/**
* @desc ゲームの状態を描画する
*/
void GameEngine::Render()const {
	//ここに描画処理を追加する

	glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, 800, 600);
	glScissor(0, 0, 800, 600);
	glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	uboLight->BufferSubData(&lightData);
	entityBuffer->Draw(meshBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBindVertexArray(vao);
	progColorFilter->UseProgram();
	Uniform::PostEffectData postEffect;
	uboPostEffect->BufferSubData(&postEffect);
	progColorFilter->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
	glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);
	fontRenderer.Draw();

}