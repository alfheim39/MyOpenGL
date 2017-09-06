/**
* @file Shader.cpp
*/
#include "Shader.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <sys/stat.h>

#pragma warning(disable:4996)

namespace Shader {
	/**
	* @desc �V�F�[�_�[�v���O�������쐬����
	* @param vsCode	���_�V�F�[�_�[�R�[�h�ւ̃|�C���^.
	* @param fsCode	�t���O�����g�V�F�[�_�[�R�[�h�ւ̃|�C���^
	* @return �쐬�����v���O�����I�u�W�F�N�g
	*/
	ProgramPtr Program::Create(const char* vsFilename, const char* fsFilename) {
		struct Impl : Program {
			Impl(){}
			~Impl(){}
		};

		ProgramPtr p = std::make_shared<Impl>();
		if (!p) {
			std::cerr << "ERROR:�v���O����" << vsFilename << "�̍쐬�Ɏ��s" << std::endl;
			return{};
		}
		p->program = CreateProgramFromFile(vsFilename, fsFilename);
		if (!p->program) {
			return{};
		}

		//�T���v���[�̐��ƈʒu���擾����
		GLint activeUniforms;
		glGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &activeUniforms);
		for (int i = 0; i < activeUniforms; ++i) {
			GLint size;
			GLenum type;
			GLchar name[128];
			glGetActiveUniform(p->program, i, sizeof(name), nullptr, &size, &type, name);
			if (type == GL_SAMPLER_2D) {
				p->samplerCount = size;
				p->samplerLocation = glGetUniformLocation(p->program, name);
				if (p->samplerLocation < 0) {
					std::cerr << "ERROR:�v���O����" << vsFilename << "�̍쐬�Ɏ��s" << std::endl;
					return{};
				}
				break;
			}
		}

		//���_�V�F�[�_�[�t�@�C�����̖�������.vert����菜�������̂��v���O�������Ƃ���
		p->name = vsFilename;
		p->name.resize(p->name.size() - 4);
		return p;
	}

	/**
	* @desc �f�X�g���N�^
	*/
	Program::~Program() {
		if (program) {
			glDeleteProgram(program);
		}
	}

	/**
	* @desc Uniform�u���b�N���o�C���f�B���O�E�|�C���g�Ɋ��蓖�Ă�
	* @param blockName		���蓖�Ă�Uniform�u���b�N�̖��O
	* @param bindingPoint	���蓖�Ă��̃o�C���f�B���O�E�|�C���g
	* @retval	��������
	* @retval	�������s
	*/
	bool Program::UniformBlockBinding(const char* blockName, GLuint bindingPoint) {
		const GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
		if (blockIndex == GL_INVALID_INDEX) {
			std::cerr << "1ERROR(" << name << "): Uniform�u���b�N`" << blockName << "�̃o�C���h�Ɏ��s " << std::endl;
			return false;
		}

		glUniformBlockBinding(program, blockIndex, bindingPoint);
		const GLenum result = glGetError();
		if (result != GL_NO_ERROR) {
			std::cerr << "2ERROR(" << name << "):Uniform�u���b�N" << blockIndex << "�̃o�C���h�Ɏ��s" << std::endl;
			return false;
		}
		return true;
	}

	/**
	* @desc �`��p�v���O�����ɐݒ肷��
	*/
	void Program::UseProgram() {
		glUseProgram(program);
		for (GLuint i = 0; i < samplerLocation; ++i) {
			glUniform1i(samplerLocation + i, i);
		}
	}

	/**
	* @desc	�e�N�X�`�����e�N�X�`���E�C���[�W�E���j�b�g�Ɋ��蓖�Ă�
	* @param unit		������̃e�N�X�`���E�C���[�W�E���j�b�g�ԍ�(GL_TEXTURE0~)
	* @param type		���蓖�Ă�e�N�X�`���̎��(GL_TEXTURE_1D,GL_TEXTURE_2D,etc)
	* @param texture	���蓖�Ă�e�N�X�`���I�u�W�F�N�g
	*/
	void Program::BindTexture(GLuint unit, GLenum type, GLuint texture)
	{
		if (unit >= GL_TEXTURE0 && unit < static_cast<GLenum>(GL_TEXTURE0 + samplerCount)) {
			glBindTexture(type, texture);
		}
	}


	//=======================================================================
	// �V�F�[�_�[�R�[�h�̃R���p�C��
	//=======================================================================
	/**
	* �V�F�[�_�R�[�h���R���p�C������
	*
	* @param type �V�F�[�_�̎��
	* @param string�@�V�F�[�_�R�[�h�ւ̃|�C���^
	* @return �쐬�����V�F�[�_�I�u�W�F�N�g
	*/
	GLuint CompileShader(GLenum type, const GLchar* string) {
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &string, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char> buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
					std::cerr << "ERROR:�V�F�[�_�[�̃R���p�C���Ɏ��s\n" << buf.data() << std::endl;
				}
			}
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	/**
	* �v���O�����I�u�W�F�N�g���쐬����
	*
	* @param vsCode ���_�V�F�[�_�R�[�h�ւ̃|�C���^
	* @param fsCode �t���O�����g�V�F�[�_�R�[�h�ւ̃|�C���^
	* @return �쐬�����v���O�����I�u�W�F�N�g
	*/
	GLuint CreateShaderProgram(const GLchar* vsCode, const GLchar* fsCode) {
		GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);
		GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);
		if (!vs || !fs) {
			return 0;
		}
		GLuint program = glCreateProgram();
		glAttachShader(program, fs);
		glDeleteShader(fs);
		glAttachShader(program, vs);
		glDeleteShader(vs);
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint infoLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char>buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetProgramInfoLog(program, infoLen, NULL, buf.data());
					std::cerr << "ERROR:�V�F�[�_�[�̃����N�Ɏ��s\n" << buf.data() << std::endl;
				}
			}
			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	/**
	* �t�@�C����ǂݍ���
	*
	* @param filename	�ǂݍ��ރt�@�C����
	* @param buf		�ǂݍ��ݐ�o�b�t�@
	* 
	* @retval true		�ǂݍ��ݐ���
	* @retval false		�ǂݍ��ݎ��s
	*/
	bool ReadFile(const char* filename, std::vector<char>& buf) {
		struct stat st;
		if (stat(filename, &st)) {
			return false;
		}
		FILE* fp = fopen(filename, "rb");
		if (!fp) {
			return false;
		}
		buf.resize(st.st_size + 1);
		const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
		fclose(fp);
		if (readSize != st.st_size) {
			return false;
		}
		buf.back() = '\0';
		return true;
	}

	/**
	* @desc �t�@�C������V�F�[�_�v���O�������쐬����
	*
	* @param vsCode		���_�V�F�[�_�t�@�C����
	* @param fsCode		�t���O�����g�V�F�[�_�t�@�C����
	*
	* @return �쐬�����v���O�����I�u�W�F�N�g
	*/
	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename) {
		std::vector<char> vsBuf;
		if (!ReadFile(vsFilename, vsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" <<
				vsFilename << "��ǂݍ��߂܂���." << std::endl;
			return 0;
		}
		std::vector<char> fsBuf;
		if (!ReadFile(fsFilename, fsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" <<
				fsFilename << "��ǂݍ��߂܂���." << std::endl;
			return 0;
		}
		return CreateShaderProgram(vsBuf.data(), fsBuf.data());
	}

}
