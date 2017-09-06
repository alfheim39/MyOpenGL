#include "Audio.h"
#include <cri_adx2le.h>
#include <iostream>
#include <cstdint>

namespace Audio {

	CriAtomExVoicePoolHn voicePool;
	CriAtomDbasId dbas = CRIATOMDBAS_ILLEGAL_ID;
	CriAtomExAcbHn acb;
	CriAtomExPlayerHn player[playerMax];

	/**
	* �I�[�f�B�I�V�X�e���p�G���[�R�[���o�b�N
	*/
	void ErrorCallback(const CriChar8* errid, CriUint32 p1, CriUint32 p2,
		CriUint32* parray) {
		const CriChar8* err = criErr_ConvertIdToMessage(errid, p1, p2);
		std::cerr << err << std::endl;
	}

	//�I�[�f�B�I�V�X�e���p�A���P�[�^
	void* Allocate(void* obj, CriUint32 size) { return new uint8_t[size]; }

	//�I�[�f�B�I�V�X�e���p�f�A���P�[�^
	void Deallocate(void* obj, void* ptr) { delete[] static_cast<uint8_t*>(ptr); }

	/**
	* �I�[�f�B�I�V�X�e��������������
	*
	* @param acfPath	ACF�t�@�C���̃p�X
	* @param acbPath	ACB�t�@�C���̃p�X
	* @param awbPath	AWB�t�@�C���̃p�X 
	* @param dspBusName D-BUS��
	*
	* return �������̐���
	*/
	bool Initialize(const char* acfPath, const char* acbPath, const char* awbPath, const char* dspBusName) {
		criErr_SetCallback(ErrorCallback);

	}
}