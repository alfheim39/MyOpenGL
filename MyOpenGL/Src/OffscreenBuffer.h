#pragma once
/**
* @file OffscreenBuffer.h
*/
#include "Texture.h"
#include <GL/glew.h>
#include <memory>

class OffscreenBuffer;
typedef std::shared_ptr<OffscreenBuffer> OffscreenBufferPtr;

/**
* @desc オフスクリーンバッファー
*/
class OffscreenBuffer {
public:
	static OffscreenBufferPtr Create(int w, int h);
	GLuint GetFramebuffer() const { return framebuffer; }//フレームバッファを取得する
	GLuint GetTexture() const { return tex->Id(); }//フレームバッファ用テクスチャを取得する

private:
	OffscreenBuffer() = default;
	OffscreenBuffer(const OffscreenBuffer&) = delete;
	OffscreenBuffer& operator = (const OffscreenBuffer&) = delete;
	~OffscreenBuffer();

private:
	TexturePtr tex;//フレームバッファ用テクスチャ
	GLuint depthbuffer = 0; //深度バッファオブジェクト
	GLuint framebuffer = 0;//フレームバッファオブジェクト
};