//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Coin.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Coin::Coin(Game* game)
    : Actor(game)
    , mCollected(false)
{
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
    Game::TILE_SIZE, Game::TILE_SIZE,
    ColliderLayer::Collectable, false);

    mDrawComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Collectables/Coin.png", Game::TILE_SIZE, Game::TILE_SIZE, 10);
}

void Coin::Kill()
{
    mColliderComponent->SetEnabled(false);
    SetState(ActorState::Destroy);
}
