#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "glew/GL/glew.h"
#include "glm/glm.hpp"

struct vertex {
	glm::vec4 pos;
	glm::vec4 color;
	glm::vec2 uv;
	glm::vec3 norm;
};

struct shader {
	GLuint program;
};

struct geometry {
	GLuint vao, vbo, ibo;	//buffers
	GLuint size;			//index count
};

struct texture {
	GLuint handle;
	unsigned width, height, channels;
};

struct light {
	glm::vec3 direction;
	glm::vec4 color;
	glm::vec4 position;
};

geometry makeGeometry(vertex * verts, size_t vertCount, unsigned* indices, size_t indxCount);

void freeGeometry(geometry& geo);

shader makeShader(const char* vertSource, const char* fragSource);

void freeShader(shader &s);

void draw(const shader &s, const geometry &g);

void setUniform(const shader& s, GLuint location, const glm::mat4& v);
void setUniform(const shader& s, GLuint location, const glm::vec3& v);
void setUniform(const shader& s, GLuint location, const glm::vec4& v);
void setUniform(const shader& s, GLuint location, const texture& v, int textureSlot);

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels);

void freeTexture(texture &t);

texture loadTexture(const char* imagePath);
