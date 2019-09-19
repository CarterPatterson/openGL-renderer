#include "Context.h"
#include "Render.h"
#include "FileManager.h"
#include "ObjectLoader.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <iostream>

int main()
{
	context game;
	game.init(640, 480, "Source");
	
	vertex triVerts[] =
	{
		{ {-.5f, -.5f, 0, 1 }, { 0, 1, 0, 1 }, { 0.f, 0.f } },
		{ { .5f, -.5f, 0, 1 }, { 1, 0, 0, 1 }, { 1.f, 0.f } },
		{ {   0,  .5f, 0, 1 }, { 0, 0, 1, 1 }, { .5f, 1.f } }
	};

	unsigned triIndices[] = { 0, 1, 2 };

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);

	vertex planeVerts[] =
	{
		{ {-.5f, -.5f, 0, 1}, { 0, 0, 1, 1 }, { 0.f, 0.f }, { 1.f, 1.f, 1.f } },
		{ { .5f, -.5f, 0, 1}, { 0, 1, 1, 1 }, { 1.f, 0.f }, { 1.f, 1.f, 1.f } },
		{ { .5f,  .5f, 0, 1}, { 1, 1, 1, 1 }, { 1.f, 1.f }, { 1.f, 1.f, 1.f } },
		{ {-.5f,  .5f, 0, 1}, { 1, 0, 1, 1 }, { 0.f, 1.f }, { 1.f, 1.f, 1.f } }
	};

	unsigned planeIndices[] = { 0, 1, 2, 2, 3, 0 };

	//geometry plane = makeGeometry(planeVerts, 4, planeIndices, 6);

	geometry cube = LoadObject("Objects/cube.obj");

	std::string basicVert = ReadFile("Shaders/basicVert.txt");

	std::string basicFrag = ReadFile("Shaders/basicFrag.txt");

	shader basicShad = makeShader(basicVert.c_str(), basicFrag.c_str());

	glm::mat4 planeModel = glm::identity<glm::mat4>();

	glm::mat4 camProj = glm::perspective(glm::radians(45.f), 640.f / 480.f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	light sun;

	sun.direction = glm::vec4{ 0, -1, 1, 1 };
	sun.color = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };

	texture planeTex = loadTexture("Textures/tex.tga");
	//texture debugTex = loadTexture("Textures/debug.png");

	setUniform(basicShad, 0, camProj);
	setUniform(basicShad, 1, camView);
	setUniform(basicShad, 3, planeTex, 0);
	setUniform(basicShad, 4, sun.direction);
	setUniform(basicShad, 5, sun.color);

	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		setUniform(basicShad, 2, planeModel);
		planeModel = glm::rotate(planeModel, glm::radians(1.f), glm::vec3(0, 1, 0));

		draw(basicShad, cube);
	}
	return 0;
}