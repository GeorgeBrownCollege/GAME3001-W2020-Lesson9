#pragma once
#ifndef __Tile__
#define __Tile__

#include <vector>

#include "DisplayObject.h"
#include "TextureManager.h"
#include "Config.h"
#include "Label.h"
#include "Scene.h"

enum TileState {
	OPEN,
	CLOSED,
	START,
	GOAL,
	UNDEFINED,
	NUM_OF_TILE_STATES
};

enum TileNeighbour
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
	NUM_OF_NEIGHBOURS
};

class Tile : public DisplayObject
{
public:
	Tile(glm::vec2 worldPosition = glm::vec2(), glm::vec2 gridPosition = glm::vec2());
	~Tile();

	// Inherited via GameObject
	virtual void draw() override;

	virtual void update() override;

	virtual void clean() override;

	// get neighbours
	Tile* up();
	Tile* down();
	Tile* right();
	Tile* left();

	// set neighbours
	void setUp(Tile* tile);
	void setDown(Tile* tile);
	void setRight(Tile* tile);
	void setLeft(Tile* tile);

	void setTileState(TileState state);
	TileState getTileState();

	void setTargetDistance(glm::vec2 goalLocation);

	glm::vec2 getGridPosition();

	float getTileValue();

	void setTileStateLabel(std::string closedOpen);

	std::vector<Tile*> getNeighbours();

private:
	float m_cost = Config::TILE_COST;
	float m_targetDist = 0.0f;
	float m_tileValue = 0.0f;
	TileState m_tileState;
	glm::vec2 m_gridPosition;
	Label* m_pValueLabel;
	Label* m_pClosedOpenLabel;

	glm::vec2 m_goalLocation;

	std::vector<Tile*> m_pNeighbours;
};


#endif /* defined (__Tile__) */