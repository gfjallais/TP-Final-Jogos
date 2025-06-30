#pragma once

#include "Actor.h"

class Cheese : public Actor
{
public:
    explicit Cheese(Game* game);

    void Kill() override;
    void CollectCheese();

private:
    class DrawSpriteComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    bool mCollected;
};