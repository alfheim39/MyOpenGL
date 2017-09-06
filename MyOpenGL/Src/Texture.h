#pragma once
/**
* @file Texture.h
*/
#include <GL/glew.h>
#include <memory>

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;///テクスチャポインタ

/**
* @desc テクスチャクラス
*/
class Texture {
public:
	//テクスチャー作成関数
	static TexturePtr Create(int width, int height, GLenum iformat, GLenum format,const void* data);
	//テクスチャー読み込み関数
	static TexturePtr LoadFromFile(const char*);

	GLuint Id() const { return texId;}
	GLsizei Width() const { return width; }
	GLsizei Height() const { return height; }

private:
	Texture();
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	GLuint texId;
	int width;
	int height;
};