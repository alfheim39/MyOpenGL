#pragma once
#include <GL/glew.h>
#include "BufferObject.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Font {
	
	struct Vertex;

	//====================
	//	�t�H���g���
	//====================
	struct FontInfo {
		//�����R�[�h
		int id = -1;
		//�t�H���g�摜�̍���e�N�X�`�����W
		glm::vec2 uv;
		//�t�H���g�摜�̃s�N�Z���T�C�Y
		glm::vec2 size;
		//�\���ʒu�����炷����
		glm::vec2 offset;
		//�J�[�\����i�߂鋗��
		float xadvance = 0;
	};

	//===============================
	//�r�b�g�}�b�v�t�H���g�`��N���X
	//===============================
	class Renderer {
	public:
		Renderer() = default;
		~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		bool Init(size_t maxChar, const glm::vec2& ss);
		bool LoadFromFile(const char* filename);

		void Scale(const glm::vec2& s) { scale = s; }
		const glm::vec2& Scale()const { return scale; }
		void Color(const glm::vec4 c);
		glm::vec4 Color()const;

		void MapBuffer();
		bool AddString(const glm::vec2& position, const char* str);
		void UnmapBuffer();
		void Draw()const;

	private:
		BufferObject vbo;
		BufferObject ibo;
		VertexArrayObject vao;
		GLsizei vboCapacity = 0;
		std::vector<FontInfo> fontList;
		std::string texFilename;
		Shader::ProgramPtr progFont;
		glm::vec2 screenSize;
		glm::vec2 screenScale;

		glm::vec2 scale = glm::vec2(1, 1);
		glm::u8vec4 color = glm::u8vec4(255, 255, 255, 255);
		GLsizei vboSize = 0;
		Vertex* pVBO = nullptr;
	};

}