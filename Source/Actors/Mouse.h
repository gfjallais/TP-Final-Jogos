//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"
#include <SDL.h>

class Mouse : public Actor
{
public:
    explicit Mouse(Game* game, float forwardSpeed = 1000.0f, float jumpSpeed = -600.0f, bool isPlayer1 = true);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void OnHandleKeyPress(const int key, const bool isPressed) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void SetIsOnWall(bool isOnWall) {
        mIsOnWall = isOnWall;
    }
    void SetWallSide(bool side) {
        mWallSide = side; // true if left, false if right
    }
    void SetSpeed(float speed) {
        mForwardSpeed = speed;
    }
    void SetIsLeaving(bool state) {
        mIsLeaving = state;
    }
    bool GetIsLeaving() {
        return mIsLeaving;
    }
    bool GetSpellMode() {
        return mSpellMode;
    }

    void CastSpell(int x, int y);

    void CollectCheese();
    bool WasChesseCollected() {
        return mCollectedCheese;
    }

    bool PressedLeft(const uint8_t* state);
    bool PressedRight(const uint8_t* state);
    bool PressedUp(const uint8_t* state);
    bool PressedDown(const uint8_t* state);

    void ToggleSpellMode();

    void PerformWallJump();

    void UpdateWallJumpLogic(const Uint8* keyState);

    void Kill() override;
    void Win();

    void ChangeToWizardSprite(bool toWizard);

    void UpdateBlockPreview(int mouseX, int mouseY);
    void DrawBlockPreview(SDL_Renderer* renderer);

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsOnWall;
    bool mWallSide;
    bool mIsDying;
    bool mCollectedCheese;
    int mWallJumpCooldown;
    bool mCanWallJump;
    bool mWasMovingAwayFromWall;
    bool mIsPlayer1;
    bool mIsLeaving;
    bool mSpellMode;
    int mSpellCount;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    Vector2 mBlockPreviewPos;
    bool mShowBlockPreview = false;
};
