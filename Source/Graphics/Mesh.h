// File Name:    Mesh.h
// Author(s):    main Boyuan Hu, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Graphics mesh data and rendering utilities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine::Graphics {

class Mesh {
public:
	Mesh();
	~Mesh();
	using SpriteIDCalculator = std::function<int(int row, int col)>;
	void BuildCustom(const std::string_view &filename);
	void BuildQuad();
	void BuildTextGrid(int maxCharacters, float charWidth = 1.0f, float charHeight = 1.0f);
	void BuildStaticGrid(
		int numCols,
		int numRows,
		float cellWidth,
		float cellHeight,
		int atlasColumns,
		int atlasRows,
		SpriteIDCalculator spriteIDCalculator
	);
	void BuildStaticText(
		const std::string &text,
		float charWidth,
		float charHeight,
		int fontAtlasColumns = 16,
		int fontAtlasRows = 16
	);

	void BuildTilemap(
		const std::vector<int> &tileIDs,
		int mapWidth,
		int mapHeight,
		float tileSize,
		int tileAtlasColumns,
		int tileAtlasRows
	);
	void Render();
	void Cleanup();

private:
	unsigned int VAO, VBO;
	unsigned int vertexCount;
	void UploadVertices(const std::vector<float> &vertices);
	glm::vec2 CalculateUVOffset(int spriteID, int cols, int rows);
};

}
