#pragma once

#include <glm.hpp>
#include "Shader.h"

class BezierSurface
{
public:
	BezierSurface();
	std::vector<std::vector<int>> controlPoints;
	int subdivisionsU = 4;
	int subdivisionsV = 4;
	bool displayControlNet = false;
	std::vector<unsigned int> netIndices;
	unsigned int VAO, VBO, EBO, netVAO, netEBO;
	std::vector<glm::vec3> vertices;
	void Draw(Shader& shader);
};
