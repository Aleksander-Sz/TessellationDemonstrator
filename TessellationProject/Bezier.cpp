#include "Bezier.h"

// BezierSurface class functions
BezierSurface::BezierSurface(glm::vec2 _location, int locationH, int locationV)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &netVAO);
	glGenBuffers(1, &netEBO);
	location.x = _location.x;
	location.y = 0.0f;
	location.z = _location.y;
	locationHorizontal = locationH;
	locationVertical = locationV;
	Mesh();
}

void BezierSurface::Mesh()
{
	static int currentMesh = 1;
	currentMesh = (currentMesh + 1) % 2;
	// Generate an array of control points for a simple Bezier surface
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			vertices.push_back(glm::vec3((i - 1.5f), 0, (j - 1.5f)));
		}
	}
	for (size_t i = 1; i < 3; i++)
	{
		for (size_t j = 1; j < 3; j++)
		{
			if(currentMesh==0)
				vertices[i * 4 + j].y = 2.0f;
			else
				vertices[i * 4 + j].y = -1.0f;
		}
	}
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(netVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			indices.push_back(i * 4 + j);
			indices.push_back(i * 4 + j + 1);
		}
	}
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			indices.push_back(j * 4 + i);
			indices.push_back((j + 1) * 4 + i);
		}
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void BezierSurface::ChangeMesh()
{
	Mesh();
}

float CalculateFactor(glm::vec3 location, glm::vec3 cameraLocation)
{
	float distance = glm::length(location - cameraLocation);
	return -16.0f * log10(distance * 0.01f) * TESS_MULTIPLIER;
}

void BezierSystem::CalculateBorderFactors(glm::vec3 cameraLocation)
{
	// Horizontal
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			glm::vec3 location = glm::vec3(-2.0f + 4.0f * j, 0.0f, i * 4.0f);
			horizontalBorderFactors[i][j] = CalculateFactor(location, cameraLocation);
		}
	}
	// Vertical
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 5; j++)
		{
			glm::vec3 location = glm::vec3(i * 4.0f, 0.0f, -2.0f + 4.0f * j);
			verticalBorderFactors[i][j] = CalculateFactor(location, cameraLocation);
		}
	}
}

void BezierSystem::ChangeMesh()
{
	surfaces[0].Mesh();
	for (size_t i = 0; i < surfaces.size(); i++)
	{
		surfaces[i].Mesh();
	}
}

void BezierSurface::Draw(Shader& shader, glm::vec3 cameraLocation)
{
	// Let's calculate the distance from the camera to the center of the surface
	glm::vec3 displacement = location - cameraLocation;
	float factor = CalculateFactor(location, cameraLocation);

	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", glm::translate(glm::mat4(1.0f), location));
	shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setFloat("tessLevelInner", factor);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glDrawArrays(GL_PATCHES, 0, 16);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}

void BezierSurface::DrawControlNet(Shader& shader, glm::vec3 cameraLocation)
{
	shader.use();
	glBindVertexArray(netVAO);
	shader.setMat4("model", glm::translate(glm::mat4(1.0f), location));
	shader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}

BezierSystem::BezierSystem()
{
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			surfaces.push_back(BezierSurface(glm::vec2(i * 3.0f, j * 3.0f), i, j));
		}
	}
}

void BezierSystem::Draw(Shader& shader, glm::vec3 cameraLocation)
{
	CalculateBorderFactors(cameraLocation);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			shader.use();
			shader.setFloat("tessLevelOuterRight", horizontalBorderFactors[i][j + 1]);
			shader.setFloat("tessLevelOuterLeft", horizontalBorderFactors[i][j]);
			shader.setFloat("tessLevelOuterBottom", verticalBorderFactors[j][i]);
			shader.setFloat("tessLevelOuterTop", verticalBorderFactors[j][i+1]);
			shader.setFloat("offsetX", j);
			shader.setFloat("offsetZ", i);
			shader.setFloat("totalShapeSize", 3.0f);
			surfaces[i*4+j].Draw(shader, cameraLocation);
		}
	}
}

void BezierSystem::DrawControlNet(Shader& shader, glm::vec3 cameraLocation)
{
	for (size_t i = 0; i < 16; i++)
	{
		surfaces[i].DrawControlNet(shader, cameraLocation);
	}
}
