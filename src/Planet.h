#pragma once
#ifndef __Planet__
#define __Planet__

#include "PathFindingDisplayObject.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "Tile.h"
#include "Scene.h"


class Planet : public PathFindingDisplayObject
{
public:
	Planet();
	~Planet();

	// Inherited via GameObject
	virtual void draw() override;

	virtual void update() override;

	virtual void clean() override;

private:
};


#endif /* defined (__Planet__) */