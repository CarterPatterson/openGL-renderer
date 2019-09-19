#include "Render.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "glm/gtc/type_ptr.hpp"
#include "glew/GL/glew.h"
#include "glm/glm.hpp"
#include <iostream>

geometry makeGeometry(vertex* verts, size_t vertCount, unsigned* indices, size_t indxCount)
{
	//create an instance of geometry
	geometry g = {};
	g.size = indxCount;

	//generate buffers
	glGenVertexArrays(1, &g.vao);	//Vertex array object
	glGenBuffers(1, &g.vbo);		//Vertex buffer object
	glGenBuffers(1, &g.ibo);		//Index buffer object

	//bind buffers
	glBindVertexArray(g.vao);
	glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.ibo);

	//populate buffers
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof (vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned), indices, GL_STATIC_DRAW);

	//decribe vertex data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)16);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)32);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)40);

	//unbind buffers (in order)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//return
	return g;
}

void freeGeometry(geometry& g)
{
	glDeleteBuffers(1, &g.vbo);
	glDeleteBuffers(1, &g.ibo);
	glDeleteBuffers(1, &g.vao);

	g = {};
}

shader makeShader(const char* vertSource, const char* fragSource)
{
	//make shader
	shader s = {};
	s.program = glCreateProgram();
	
	//create shader
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	//compile shader
	glShaderSource(vert, 1, &vertSource, 0);
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);
	//TODO: Error checking
	GLint vCompiled;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vCompiled);
	if (vCompiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vert, 1024, &log_length, message);
		std::cout << message << std::endl;
	}

	glCompileShader(frag);
	//TODO: Error checking
	GLint fCompiled;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &fCompiled);
	if (fCompiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(frag, 1024, &log_length, message);
		std::cout << message << std::endl;
	}

	//attach shader
	glAttachShader(s.program, vert);
	glAttachShader(s.program, frag);

	//link shader
	glLinkProgram(s.program);
	//TODO: Error checking

	//delete shader
	glDeleteShader(vert);
	glDeleteShader(frag);

	//return object
	return s;
}

void freeShader(shader& s)
{
	glDeleteProgram(s.program);
	s = {};
}

void draw(const shader& s, const geometry& g)
{
	//bind shader program
	glUseProgram(s.program);

	//bind the vao
	glBindVertexArray(g.vao);

	//draw
	glDrawElements(GL_TRIANGLES, g.size, GL_UNSIGNED_INT, 0);
}

void setUniform(const shader &s, GLuint location, const glm::mat4 &v)
{
	glProgramUniformMatrix4fv(s.program, location, 1, GL_FALSE, glm::value_ptr(v));
}

void setUniform(const shader& s, GLuint location, const texture& v, int textureSlot)
{
	//specify the texture slot we're working with
	glActiveTexture(GL_TEXTURE0 + textureSlot);

	//bind texture to slot
	glBindTexture(GL_TEXTURE_2D, v.handle);

	//assign the uniform to shader
	glProgramUniform1i(s.program, location, textureSlot);
}

void setUniform(const shader& shad, GLuint location, const glm::vec3& value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}

void setUniform(const shader& shad, GLuint location, const glm::vec4& value)
{
	glProgramUniform4fv(shad.program, location, 1, glm::value_ptr(value));
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels)
{
	GLenum oglFormat = GL_RGBA;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;
		break;
	case 2:
		oglFormat = GL_RG;
		break;
	case 3:
		oglFormat = GL_RGB;
		break;
	case 4:
		oglFormat = GL_RGBA;
		break;
	default:
		//TODO: fatal error, halt
		break;
	}

	//Gen and bind texture
	texture t = { 0, width, height, channels };
	glGenTextures(1, &t.handle);
	glBindTexture(GL_TEXTURE_2D, t.handle);

	//buffer/send actual data
	glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);

	//describe how texture will be used
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	return t;
}

void freeTexture(texture &t)
{
	glDeleteTextures(1, &t.handle);
	t = {};
}

texture loadTexture(const char* imagePath)
{
	int imageWidth, imageHeight, imageFormat;
	unsigned char* rawPixelData = nullptr;

	//tell stb image to load image
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(imagePath, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	//TODO: ensure that rawPixelData is NOT NULL. if it is, the image failed to load

	//pass data to makeTexture
	texture t = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);

	//fee the image
	stbi_image_free(rawPixelData);

	return t;
}
