#pragma once
/**
* @file UnifromBuffer.h
*/
#include <GL/glew.h>
#include <string>
#include <memory>

class UniformBuffer;
typedef std::shared_ptr<UniformBuffer> UniformBufferPtr;//UBO�^�|�C���^

/**
* @desc UBO�N���X
*/
class UniformBuffer {
public:
	static UniformBufferPtr Create(GLsizeiptr size,GLuint bindingPoint,const char* name);
	bool BufferSubData(const GLvoid* data, GLintptr offset = 0, GLsizeiptr size = 0);
	void BindBufferRange(GLintptr offset, GLsizeiptr size)const;
	void* MapBuffer()const;
	void UnmapBuffer()const;

	GLsizeiptr Size() const { return size; }

private:
	UniformBuffer() = default;
	~UniformBuffer();
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

private:
	GLuint ubo = 0;			//UniformBufferObject
	GLsizeiptr size = 0;	//UBO�o�C�g�T�C�Y
	GLuint bindingPoint;	//UBO�̊��蓖�ĂĐ�o�C���f�B���O�|�C���g
	std::string name;		//UBO��

};