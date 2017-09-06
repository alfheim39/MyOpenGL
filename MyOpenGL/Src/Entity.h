#pragma once
#include <GL/glew.h>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <functional>
#include <vector>

namespace Entity {

	class Entity;
	class Buffer;
	typedef std::shared_ptr<Buffer> BufferPtr;//�G���f�B���O�o�b�t�@�|�C���^�^

	//�Փˉ����n���h���^
	typedef std::function<void(Entity&, Entity&)> CollisionHandlerType;
	//�O���[�vID�̍ő�l
	static const int maxGroupId = 31;
	/**
	* @desc �Փ˔���`��
	*/
	struct CollisionData {
		glm::vec3 min;
		glm::vec3 max;
	};


/**
* @desc �G���e�B�e�B
*/
	class Entity {
		friend class Buffer;


	public:
		//��ԍX�V�֐��^
		typedef std::function<
			void(Entity&, void*, double, const glm::mat4&, const glm::mat4&)> UpdateFuncType;

		void Position(const glm::vec3& v) { position = v; }
		const glm::vec3& Position()const { return position; }
		void Rotation(const glm::quat& q) { rotation = q; }
		const glm::quat& Rotation()const { return rotation; }
		void Scale(const glm::vec3& v) { scale = v; }
		const glm::vec3& Scale()const { return scale; }
		void Velocity(const glm::vec3& v) { velocity = v; }
		const glm::vec3& Velocity()const { return velocity; }
		void UpdateFunc(const UpdateFuncType& func) { updateFunc = func; }
		const UpdateFuncType& UpdateFunc()const { return updateFunc; }
		void Collision(const CollisionData& c) { colLocal = c; }
		const CollisionData& Collision()const { return colLocal; }


		glm::mat4 TRSMatrix()const;
		int GroupId()const { return groupId; }
		void Destroy();

	private:
		Entity() = default;
		~Entity() = default;
		Entity(const Entity&) = default;
		Entity& operator=(const Entity&) = default;


	private:
		int groupId = -1;//�O���[�vID
		Buffer* pBuffer = nullptr;//��������Buffer�N���X�ւ̃|�C���^
		glm::vec3 position;//���W
		glm::vec3 scale = glm::vec3(1, 1, 1);//�g�嗦
		glm::quat rotation;//��]
		glm::vec3 velocity;//���x
		UpdateFuncType updateFunc;//��ԍX�V�֐�
		Mesh::MeshPtr mesh;//�G���e�B�e�B��`�悷��Ƃ��Ɏg���郁�b�V���f�[�^
		TexturePtr texture;//�G���e�B�e�B��`�悷��Ƃ��Ɏg����e�N�X�`��
		Shader::ProgramPtr program;//�G���e�B�e�B��`�悷��Ƃ��Ɏg����V�F�[�_
		GLintptr uboOffset;//UBO�̃G���e�B�e�B�p�̈�ւ̃o�C�g�I�t�Z�b�g
		bool isActive = false;//�A�N�e�B�u�ȃG���e�B�e�B�Ȃ�true,��A�N�e�B�u�Ȃ�false
		CollisionData colLocal;//���[�J�����W�n�̏Փˌ`��
		CollisionData ColWorld;//���[���h���W�n�̏Փˌ`��
		
	};

	/**
	* @desc �G���e�B�e�B�o�b�t�@
	*/
	class Buffer {
	public:
		static BufferPtr Create(size_t maxEntityCount, GLsizeiptr ubSizeEntity, int bindingPoint, const char* name);
		Entity* AddEntity(int groupId, const glm::vec3& pos, const Mesh::MeshPtr& m, const TexturePtr& t,
			const Shader::ProgramPtr& p, Entity::UpdateFuncType func);
		void RemoveEntity(Entity* entity);
		void Update(double delta, const glm::mat4& matView, const glm::mat4& matProj);
		void Draw(const Mesh::BufferPtr& meshBuffer)const;
		void CollisionHandler(int gid0, int gid1, CollisionHandlerType handler);
		const CollisionHandlerType& CollisionHandler(int gid0, int gid1)const;
		void ClearCollisionHandlerList();

	private:
		Buffer() = default;
		~Buffer() = default;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

	private:
		//�G���e�B�e�B�p�����N���X�g
		struct Link {
			void Insert(Link* e);
			void Remove();
			Link* prev = this;
			Link* next = this;
		};

		//�����N���G���e�B�e�B
		struct LinkEntity:public Link,public Entity{};
		//�����N���G���e�B�e�B�z��̍폜�p�֐��I�u�W�F�N�g
		struct EntityArrayDeleter { void operator()(LinkEntity* p) { delete[] p; } };

		std::unique_ptr<LinkEntity[], EntityArrayDeleter> buffer;//�G���e�B�e�B�̔z��
		size_t bufferSize;//�G���e�B�e�B�̑���
		Link freeList;//���g�p�̃G���e�B�e�B�̃����N���X�g
		Link activeList[maxGroupId + 1];//�g�p���̃G���e�B�e�B�̃����N���X�g
		GLsizeiptr ubSizePerEntity;//�e�G���e�B�e�B���g����Uniform Buffer�̃o�C�g��
		UniformBufferPtr ubo;//�G���e�B�e�B�pUBO
		Link* itrUpdate = nullptr;//Update��Removeentity�̑��ݍ�p�ɑΉ����邽�߂̃C�e���[�^�[
		Link* itrUpdateRhs = nullptr;

		struct CollisionHandlerInfo {
			int groupId[2];
			CollisionHandlerType handler;
		};

		std::vector<CollisionHandlerInfo> collisionHandlerList;
	};



}