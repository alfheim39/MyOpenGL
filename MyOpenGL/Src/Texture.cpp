/**
* @file Texture.cpp
*/
#include "Texture.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#pragma warning(disable:4996)

/**
* @desc バイト列から数値を復元する
*
* @param p		バイト列へのポインタ
* @param offset	数値のオフセット
* @param size	数値のバイト数(1〜4)
*
* @return		復元した数値
*/
uint32_t Get(const uint8_t* p, size_t offset, size_t size) {
	uint32_t n = 0;
	p += offset;
	for (size_t i = 0; i < size; ++i) {
		n += p[i] << (i * 8);
	}
	return n;
}

/**
* @desc コンストラクタ
*/
Texture::Texture():texId(0),width(0),height(0){}

/**
* @desc デストラクタ
*/
Texture::~Texture() {
	if (texId) {
		glDeleteTextures(1, &texId);
	}
}

/**
* @desc 2Dテクスチャを作成する
*
* @param width		テクスチャの幅（ピクセル数）
* @param height		テクスチャの高さ（ピクセル数）
* @param iformat	テクスチャのデータ形式
* @param format		アクセスする要素
* @param data		テクスチャデータへのポインタ
*
* @return 作成に成功した場合はテクスチャポインタを返す
*				失敗した場合はnullptrを返す
*/
TexturePtr Texture::Create(int width, int height, GLenum iformat, GLenum format, const void* data) {
	
	struct Impl:Texture{
		Impl(){}
		~Impl(){}
	};
	TexturePtr p = std::make_shared<Impl>();

	//テクスチャを作成
	p->width = width;
	p->height = height;
	glGenTextures(1, &p->texId);

	glBindTexture(GL_TEXTURE_2D, p->texId);
	glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "ERROR テクスチャ作成に失敗 : 0x" << std::hex << result << std::endl;
		return{};
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return p;
}

/**
* @desc ファイルから2Dテクスチャを読み込む
*
* @param filename		ファイル名
*
* @return 作成に成功した場合はテクスチャポインタを返す
*				失敗した場合はnullptrを返す
*/
TexturePtr Texture::LoadFromFile(const char* filename) {
	
	//サイズ判定をパスした場合、ファイルを開く
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return{};
	}

	//ファイルサイズ取得処理
	struct stat st;
	if (stat(filename, &st)) {
		return{};
	}
	//BMPファイルの情報を保持できるサイズかを調べる
	const size_t bmpFileHeaderSize = 14;//ビットマップファイルヘッダのバイト数
	const size_t windowV1HeaderSize = 40;//ビットマップ情報ヘッダのバイト数
	if (st.st_size < bmpFileHeaderSize + windowV1HeaderSize) {
		return{};
	}
	
	//読み込み
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	if (readSize != st.st_size) {
		return{};
	}

	//本当にBMPファイルかどうかを検査
	const uint8_t* pHeader = buf.data();
	if (pHeader[0] != 'B' || pHeader[1] != 'M') {
		return{};
	}

	//詳しく検査
	const size_t offsetBytes = Get(pHeader, 10, 4);
	const uint32_t infoSize = Get(pHeader, 14, 4);
	const uint32_t width = Get(pHeader, 18, 4);
	const uint32_t height = Get(pHeader, 22, 4);
	const uint32_t bitCount = Get(pHeader, 28, 4);
	const uint32_t compression = Get(pHeader, 30, 4);
	const size_t pixelBytes = bitCount / 8;
	if (infoSize != windowV1HeaderSize || bitCount != 24 || compression || (width * pixelBytes) % 4) {
		return {};
	}


	//画像データが正しく格納されているか
	const size_t imageSize = width * height * pixelBytes;
	if (buf.size() < offsetBytes + imageSize) {
		return {};
	}

	return Create(width, height, GL_RGBA, GL_BGR, buf.data() + offsetBytes);
	
}