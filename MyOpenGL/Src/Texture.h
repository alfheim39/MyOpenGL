#pragma once
/**
* @file Texture.h
*/
#include <GL/glew.h>
#include <memory>

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;///�e�N�X�`���|�C���^

/**
* @desc �e�N�X�`���N���X
*/
class Texture {
public:
	//�e�N�X�`���[�쐬�֐�
	static TexturePtr Create(int width, int height, GLenum iformat, GLenum format,const void* data);
	//�e�N�X�`���[�ǂݍ��݊֐�
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