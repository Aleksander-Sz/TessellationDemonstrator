#include "Bezier.h"

// BezierSurface class functions
BezierSurface::BezierSurface()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &netVAO);
	glGenBuffers(1, &netEBO);

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
}

void BezierSurface::ChangeMesh()
{
	Mesh();
}

void BezierSurface::Draw(Shader& shader)
{
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", glm::mat4(1.0f));
	shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setFloat("tessLevelOuter", 5.0f);
	shader.setFloat("tessLevelInner", 10.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glDrawArrays(GL_PATCHES, 0, 16);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}