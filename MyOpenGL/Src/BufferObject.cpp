#include "BufferObject.h"

/**
* @desc バッファオブジェクトを作成する
* @param バッファオブジェクトの種類
* @param 頂点データのサイズ
* @param 頂点データへのポインタ
* @param バッファオブジェクトのアクセスタイプ
*/
void BufferObject::Init(GLenum target,GLsizeiptr size,const GLvoid* data,GLenum usage) {
	Destroy();
	glGenBuffers(1, &id);
	glBindBuffer(target, id);
	glBindBuffer(target, id);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
}

/**
* @desc BufferObjectを破壊する
*/
void BufferObject::Destroy() {
	if (id) {
		glDeleteBuffers(1, &id);
		id = 0;
	}
}

/**
* @desc VAOを作成する
* @param 頂点バッファオブジェクトのID
* @param インデックスバッファオブジェクトのID
*/
void VertexArrayObject::Init(GLuint vbo, GLuint ibo) {
	Destroy();
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
}

/**
* @desc VAOを破棄する
*/
void VertexArrayObject::Destroy() {
	if (id) {
		glDeleteVertexArrays(1, &id);
		id = 0;
	}
}

/**
* @desc 頂点アトリビュートを設定する
* @param 頂点アトリビュートのインデックス
* @param 頂点アトリビュートの要素数
* @param 頂点アトリビュートの型
* @param GL_TYPE=要素を正規化する GL_FALSE=正規化しない
* @param 次の頂点データまでのバイト数
* @param 頂点データ先頭からのバイトオフセット
*/
void VertexArrayObject::VertexAttribPointer(GLuint index, GLint size, GLenum type,
	GLboolean normalized, GLsizei stride, size_t offset) {
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<GLvoid*>(offset));
}

/**
* @desc VAOを処理対象にする
*/
void VertexArrayObject::Bind()const {
	glBindVertexArray(id);
}

/**
* @desc VAOを処理対象から外す
*/
void VertexArrayObject::Unbind()const {
	glBindVertexArray(0);
}