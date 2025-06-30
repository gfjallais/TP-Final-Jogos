#include "Cheese.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Cheese::Cheese(Game* game)
    : Actor(game)
{
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
    Game::TILE_SIZE, Game::TILE_SIZE,
    ColliderLayer::Collectable, false);

    mDrawComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Collectables/Cheese.png", Game::TILE_SIZE, Game::TILE_SIZE, 10);
}

void Cheese::Kill()
{
    mColliderComponent->SetEnabled(false);
    SetState(ActorState::Destroy);
}
