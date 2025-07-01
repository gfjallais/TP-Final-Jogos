//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include <algorithm>

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE - 4.0f,Game::TILE_SIZE,
                                                   ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
                                              "../Assets/Sprites/Mario/Mario.png",
                                              "../Assets/Sprites/Mario/Mario.json");

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("idle", {1});
    mDrawComponent->AddAnimation("jump", {2});
    mDrawComponent->AddAnimation("run", {3, 4, 5});
    mDrawComponent->AddAnimation("win", {7});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mCollectedCheese = false;
}

void Mario::OnProcessInput(const uint8_t* state)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    if (state[SDL_SCANCODE_D])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
        mRotation = 0.0f;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A])
    {
        mIsRunning = false;
    }
}

void Mario::OnHandleKeyPress(const int key, const bool isPressed)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump
    if ((key == SDLK_SPACE || key == 119) && isPressed && (mIsOnGround || (mCanWallJump && !mIsOnWall)))
    {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
        mCanWallJump = false;
        mWallJumpCooldown = 0;
        mIsOnGround = false;
        SetSpeed(700.0f);

        // Play jump sound
//        mGame->GetAudio()->PlaySound("Jump.wav");
    }
}

void Mario::OnUpdate(float deltaTime)
{
    SetSpeed(1000.0f);
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0) {
        mIsOnGround = false;
    }

    if(mWallJumpCooldown > 0) {
        mCanWallJump = true;
        mWallJumpCooldown--;
    } else {
        mCanWallJump = false;
    }

    // Limit Mario's position to the camera view
    mPosition.x = Math::Max(mPosition.x, mGame->GetCameraPos().x);

    // Kill mario if he falls below the screen
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y > mGame->GetWindowHeight()) {
        Kill();
    }


    if (mGame->GetGamePlayState() == Game::GamePlayState::Leaving)
    {
        mState = ActorState::Destroy;
        auto gameScene = mGame->GetGameSceneSequence();
        auto it = (std::find(gameScene.begin(), gameScene.end(), mGame->GetGameScene()) - gameScene.begin() + 1) % gameScene.size();
        mGame->SetGameScene(gameScene[it]);
        return;
    }

    ManageAnimations();
}

void Mario::ManageAnimations()
{
    if(mIsDying)
    {
        mDrawComponent->SetAnimation("Dead");
    }
    else if (mIsOnGround && mIsRunning)
    {
        mDrawComponent->SetAnimation("run");
    }
    else if (mIsOnGround && !mIsRunning)
    {
        mDrawComponent->SetAnimation("idle");
    }
    else if (!mIsOnGround)
    {
        mDrawComponent->SetAnimation("jump");
    }
}

void Mario::Kill()
{
    mIsDying = true;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    mDrawComponent->SetAnimation("Dead");

    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("Dead.wav");

    mGame->ResetGameScene(3.5f); // Reset the game scene after 3 seconds
}

void Mario::Win()
{
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Kill();
    }
    if(other->GetLayer() == ColliderLayer::Blocks) {
        mIsOnWall = true;
        mWallJumpCooldown = 20;
    }
    if (other->GetLayer() == ColliderLayer::Collectable)
    {
        CollectCheese();
        other->GetOwner()->Kill();
    }
    if (other->GetLayer() == ColliderLayer::Exit && mCollectedCheese) {
        mGame->GetAudio()->PlaySound("Victoire.wav");
        mGame->SetGamePlayState(Game::GamePlayState::Leaving);
    }
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        other->GetOwner()->Kill();
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed / 2.5f));

        // Play jump sound
        mGame->GetAudio()->PlaySound("Stomp.wav");
    }
    else if (other->GetLayer() == ColliderLayer::Blocks)
    {
        if (!mIsOnGround) {
//            mGame->GetAudio()->PlaySound("Bump.wav");
//
//            Block* block = static_cast<Block*>(other->GetOwner());
//            block->OnBump();
        }
    }
    if (other->GetLayer() == ColliderLayer::Collectable)
    {
        CollectCheese();
        other->GetOwner()->Kill();
    }
    if (other->GetLayer() == ColliderLayer::Exit && mCollectedCheese) {
        mGame->GetAudio()->PlaySound("Victoire.wav");
        mGame->SetGamePlayState(Game::GamePlayState::Leaving);
    }
}

void Mario::CollectCheese() {
    mCollectedCheese = true;
    mForwardSpeed = 600.0f;
    mJumpSpeed = -500.0f;
    mGame->GetAudio()->PlaySound("cheese.wav");
}
