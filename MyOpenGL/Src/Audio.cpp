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
	* オーディオシステム用エラーコールバック
	*/
	void ErrorCallback(const CriChar8* errid, CriUint32 p1, CriUint32 p2,
		CriUint32* parray) {
		const CriChar8* err = criErr_ConvertIdToMessage(errid, p1, p2);
		std::cerr << err << std::endl;
	}

	//オーディオシステム用アロケータ
	void* Allocate(void* obj, CriUint32 size) { return new uint8_t[size]; }

	//オーディオシステム用デアロケータ
	void Deallocate(void* obj, void* ptr) { delete[] static_cast<uint8_t*>(ptr); }

	/**
	* オーディオシステムを初期化する
	*
	* @param acfPath	ACFファイルのパス
	* @param acbPath	ACBファイルのパス
	* @param awbPath	AWBファイルのパス 
	* @param dspBusName D-BUS名
	*
	* return 初期化の成否
	*/
	bool Initialize(const char* acfPath, const char* acbPath, const char* awbPath, const char* dspBusName) {
		criErr_SetCallback(ErrorCallback);

	}
}