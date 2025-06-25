//
// Created by Lucas N. Ferreira on 30/09/23.
//

#pragma once

#include "Actor.h"

class Coin : public Actor
{
public:
    explicit Coin(Game* game);

    void Kill() override;
    void CollectCoin();

private:
    class DrawSpriteComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    bool mCollected;
};