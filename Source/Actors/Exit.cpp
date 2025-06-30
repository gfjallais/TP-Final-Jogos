//
// Created by gfjallais on 30/06/2025.
//
#include "Exit.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Exit::Exit(Game* game)
        : Actor(game)
{
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE, Game::TILE_SIZE,
                                                   ColliderLayer::Exit, false);

    mDrawComponent = new DrawSpriteComponent(this, "../Assets/Sprites/exit.png", Game::TILE_SIZE, Game::TILE_SIZE, 10);
}