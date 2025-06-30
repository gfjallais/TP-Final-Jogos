//
// Created by gfjallais on 30/06/2025.
//

#ifndef P4_SUPER_MARIO_BROS_EXIT_H
#define P4_SUPER_MARIO_BROS_EXIT_H

#include "Actor.h"

class Exit : public Actor
{
public:
    explicit Exit(Game* game);

private:
    class DrawSpriteComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};

#endif //P4_SUPER_MARIO_BROS_EXIT_H
