#pragma once
#include <glm/glm.hpp>

namespace Uniform {
	//���_�V�F�[�_�[�̃p�����[�^�[�^
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;

		glm::vec4 lightPosition;
		glm::vec4 lightColor;
		glm::vec4 ambientColor;
	};

	/**
	* ���C�g�f�[�^�i�_�����j
	*/
	struct PointLight {
		glm::vec4 position;//���W�i���[���h���W�j
		glm::vec4 color;//���邳
	};

	const int maxLightCount = 4;	//���C�g�̐�

	/**
	* ���C�e�B���O�p�����[�^�[
	*/
	struct LightData {
		glm::vec4 ambientColor;//����
		PointLight light[maxLightCount];//���C�g�̃��X�g
	};

	/**
	* @desc �|�X�g�G�t�F�N�g�f�[�^
	*/
	struct PostEffectData {
		glm::mat4x4 matColor; //�F�ϊ��s��
	};
}