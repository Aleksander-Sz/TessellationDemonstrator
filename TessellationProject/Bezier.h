#pragma once

#include <glm.hpp>
#include "Shader.h"

#define TESS_MULTIPLIER 1.0f

class BezierSurface
{
public:
	BezierSurface(glm::vec2 _location, int locationH, int locationV);
	void Mesh();
	void ChangeMesh();
	glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
	std::vector<std::vector<int>> controlPoints;
	int locationHorizontal;
	int locationVertical;
	bool displayControlNet = false;
	std::vector<unsigned int> netIndices;
	unsigned int VAO, VBO, EBO, netVAO, netEBO;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	void Draw(Shader& shader, glm::vec3 cameraLocation);
	void DrawControlNet(Shader& shader, glm::vec3 cameraLocation);
};

class BezierSystem
{
public:
	BezierSystem();
	std::vector<BezierSurface> surfaces;
	void Draw(Shader& shader, glm::vec3 cameraLocation);
	void DrawControlNet(Shader& shader, glm::vec3 cameraLocation);
	void CalculateBorderFactors(glm::vec3 cameraLocation);
	float horizontalBorderFactors[4][5];
	float verticalBorderFactors[4][5];
	void ChangeMesh();
};