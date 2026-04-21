// File Name:    Mesh.cpp
// Author(s):    main Boyuan Hu, secondary Taro Omiya, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Graphics mesh data and rendering utilities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Graphics/Mesh.h"

#include <glbinding/gl/gl.h>
#include <iterator>

using namespace gl;

namespace RassEngine::Graphics {

Mesh::Mesh() : VAO(0), VBO(0), vertexCount(0) {}

Mesh::~Mesh() {
	Cleanup();
}

void Mesh::BuildQuad() {
	// Define vertices for a quad (two triangles)
	float vertices[] = {
		// Positions        // Colors           // Texture Coords
		-0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // Top-left
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
		 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // Bottom-right
		-0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // Top-left
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
		 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f // Bottom-right
	};

	vertexCount = 6;
	GLsizei stride = 8 * sizeof(float);
	// Generate and bind VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	// Upload vertex data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
	glEnableVertexAttribArray(0);
	// Colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::Render() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
}

void Mesh::Cleanup() {
	if(VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	if(VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
}
void Mesh::BuildStaticGrid(
	int numCols,
	int numRows,
	float cellWidth,
	float cellHeight,
	int atlasColumns,
	int atlasRows,
	SpriteIDCalculator spriteIDCalculator
) {
	std::vector<float> vertices;
	vertices.reserve(numCols * numRows * 6 * 8);

	glm::vec2 uvSize(1.0f / atlasColumns, 1.0f / atlasRows);

	for(int row = 0; row < numRows; ++row) {
		for(int col = 0; col < numCols; ++col) {
			//calculate quad position in world space
			float x0 = col * cellWidth;
			float x1 = x0 + cellWidth;
			float y0 = row * cellHeight;
			float y1 = y0 + cellHeight;

			// === use map col&row to get spriteID ===
			int spriteID = spriteIDCalculator(row, col);

			if(spriteID < 0) continue;  //Skip empty tiles

			// calculate atlas position using spriteID
			int atlasCol = spriteID % atlasColumns;
			int atlasRow = spriteID / atlasColumns;

			// calculate UV offset for this sprite in the atlas
			glm::vec2 uvOffset(
				static_cast<float>(atlasCol) / atlasColumns,
				//static_cast<float>(atlasRow) / atlasRows
				1.0f - static_cast<float>(atlasRow + 1) / atlasRows
			);

			// build quad
			float quad[] = {
				// Position          Color              UV (prebaked)
				x0, y1, 0.0f,  1.0f, 1.0f, 1.0f,  uvOffset.x,              uvOffset.y + uvSize.y,
				x0, y0, 0.0f,  1.0f, 1.0f, 1.0f,  uvOffset.x,              uvOffset.y,
				x1, y0, 0.0f,  1.0f, 1.0f, 1.0f,  uvOffset.x + uvSize.x,   uvOffset.y,
				x0, y1, 0.0f,  1.0f, 1.0f, 1.0f,  uvOffset.x,              uvOffset.y + uvSize.y,
				x1, y0, 0.0f,  1.0f, 1.0f, 1.0f,  uvOffset.x + uvSize.x,   uvOffset.y,
				x1, y1, 0.0f,  1.0f, 1.0f, 1.0f,  uvOffset.x + uvSize.x,   uvOffset.y + uvSize.y,
			};

			vertices.insert(vertices.end(), std::begin(quad), std::end(quad));
		}
	}

	vertexCount = vertices.size() / 8;
	UploadVertices(vertices);
}
void Mesh::UploadVertices(const std::vector<float> &vertices) {
	// Generate VAO and VBO if not already created
	if(VAO == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
		vertices.data(), GL_STATIC_DRAW);

	GLsizei stride = 8 * sizeof(float);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	vertexCount = static_cast<unsigned int>(vertices.size() / 8);
	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void Mesh::BuildStaticText(
	const std::string &text,
	float charWidth,
	float charHeight,
	int fontAtlasColumns,
	int fontAtlasRows
) {
	BuildStaticGrid(
		text.length(), 1,
		charWidth, charHeight,
		fontAtlasColumns, fontAtlasRows,
		[&text] (int row, int col) -> int {
			return text[col] - 32;
		}
	);
}

void Mesh::BuildTilemap(
	const std::vector<int> &tileIDs,
	int mapWidth,
	int mapHeight,
	float tileSize,
	int tileAtlasColumns,
	int tileAtlasRows
) {
	BuildStaticGrid(
		mapWidth, mapHeight,
		tileSize, tileSize,
		tileAtlasColumns, tileAtlasRows,
		[&tileIDs, mapWidth] (int row, int col) -> int {
			int index = row * mapWidth + col;
			return tileIDs[index]-1;
		}
	);
}
void Mesh::BuildTextGrid(int maxCharacters, float charWidth, float charHeight) {
	std::vector<float> vertices;
	vertices.reserve(maxCharacters * 6 * 8);  // each character quad has 6 vertices, each vertex has 8 attributes (position, color, tex coords)

	for(int i = 0; i < maxCharacters; ++i) {
		float x0 = i * charWidth;
		// position of the quad for each character, will be used in shader to calculate final position based on char index
		float x1 = x0 + charWidth;
		float y0 = 0.0f;
		float y1 = charHeight;
		float z = 0.0f;

		float r = 1.0f, g = 1.0f, b = 1.0f;

		// UV fixed for each character quad, actual offset will be calculated in shader based on char index
		float quad[] = {
			//position        // color            // tex coords
			x0, y1, z,       r, g, b,     0.0f, 1.0f,// Top-left
			x0, y0, z,       r, g, b,     0.0f, 0.0f,// Bottom-left
			x1, y0, z,       r, g, b,     1.0f, 0.0f,// Bottom-right
			x0, y1, z,       r, g, b,     0.0f, 1.0f,// Top-left
			x1, y0, z,       r, g, b,     1.0f, 0.0f,// Bottom-right
			x1, y1, z,       r, g, b,     1.0f, 1.0f,// Top-right
		};

		vertices.insert(vertices.end(), std::begin(quad), std::end(quad));
	}

	vertexCount = maxCharacters * 6;

	if(VAO == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
		vertices.data(), GL_STATIC_DRAW);  //static draw since text grid won't change

	GLsizei stride = 8 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

}
