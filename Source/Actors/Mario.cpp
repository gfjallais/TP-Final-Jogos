//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include <algorithm>

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed, const bool isPlayer1)
        : Actor(game)
        , mIsRunning(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mIsPlayer1(isPlayer1)
        , mIsLeaving(false)
        , mSpellMode(false)
        , mSpellCount(2)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE - 4.0f,Game::TILE_SIZE,
                                                   ColliderLayer::Player);
    std::string player1Sprite = "../Assets/Sprites/Mouse/Mouse1.png";
    std::string player2Sprite = "../Assets/Sprites/Mouse/Mouse2.png";

    mDrawComponent = new DrawAnimatedComponent(this,
                                              isPlayer1 ? player1Sprite : player2Sprite,
                                              "../Assets/Sprites/Mouse/Mouse.json");

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("idle", {1});
    mDrawComponent->AddAnimation("jump", {2});
    mDrawComponent->AddAnimation("run", {3, 4, 5, 6, 7});
    mDrawComponent->AddAnimation("win", {8});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mCollectedCheese = false;
}

bool Mario::PressedLeft(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_A];
    }
    return state[SDL_SCANCODE_LEFT];
}

bool Mario::PressedRight(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_D];
    }
    return state[SDL_SCANCODE_RIGHT];
}

bool Mario::PressedUp(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_W];
    }
    return state[SDL_SCANCODE_UP];
}

bool Mario::PressedDown(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_S];
    }
    return state[SDL_SCANCODE_DOWN];
}

void Mario::OnProcessInput(const uint8_t* state)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;
    if(mSpellMode) return;

    if (PressedRight(state))
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed);
        mRotation = 0.0f;
        mIsRunning = true;
    }

    if (PressedLeft(state))
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if ((state[SDL_SCANCODE_X] && mIsPlayer1) || (state[SDL_SCANCODE_M] && !mIsPlayer1)) {
        if(mSpellCount > 0) {
            ToggleSpellMode();
            SDL_Log("Toggled spellMode %d", mSpellMode);
        }
    }

    if (!PressedLeft(state) && !PressedRight(state))
    {
        mIsRunning = false;
    }
}

void Mario::CastSpell(int x, int y) {
    SDL_Log("Cast Spell Called");
    SDL_Log("%d", mSpellCount);
    if(mSpellCount > 0) {
        Block* block = new Block(mGame, "../Assets/Sprites/Blocks/rock.png", false);
        block->SetPosition(Vector2(x, y));
        mSpellCount--;
        ToggleSpellMode();
    }
}

void Mario::OnHandleKeyPress(const int key, const bool isPressed)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump
    if (((key == SDLK_w && mIsPlayer1) || (key == SDLK_UP && !mIsPlayer1)) && isPressed && (mIsOnGround || (mCanWallJump && !mIsOnWall)))
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
    if(mGame->AlivePlayers() == 1) {
        mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    }
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
    }
    if (other->GetLayer() == ColliderLayer::Exit && mCollectedCheese) {
        if(GetIsLeaving()) return;

        SetIsLeaving(true);

        SDL_Log("%d", mGame->PlayersLeaving());
        SDL_Log("%d", mGame->AlivePlayers());

        if(mGame->PlayersLeaving() == mGame->AlivePlayers()) {
            mGame->SetGamePlayState(Game::GamePlayState::Leaving);
            mGame->GetAudio()->PlaySound("victory.wav");
        }

        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);
        mDrawComponent->SetEnabled(false);
        mState = ActorState::Destroy;
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
    }
    if (other->GetLayer() == ColliderLayer::Exit && mCollectedCheese) {
        if(GetIsLeaving()) return;

        SetIsLeaving(true);

        SDL_Log("%d", mGame->PlayersLeaving());
        SDL_Log("%d", mGame->AlivePlayers());

        if(mGame->PlayersLeaving() == mGame->AlivePlayers()) {
            mGame->SetGamePlayState(Game::GamePlayState::Leaving);
            mGame->GetAudio()->PlaySound("victory.wav");
        }

        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);
        mDrawComponent->SetEnabled(false);
        mState = ActorState::Destroy;
    }
}

void Mario::CollectCheese() {
    if (!mCollectedCheese){
        mCollectedCheese = true;
        mForwardSpeed = 600.0f;
        mJumpSpeed = -500.0f;
        mGame->GetAudio()->PlaySound("cheese.wav");
        // Change sprite sheet to cheese version
        std::string cheeseSprite = mIsPlayer1 ? "../Assets/Sprites/Mouse/Mouse1_cheese.png" : "../Assets/Sprites/Mouse/Mouse2_cheese.png";
        mDrawComponent->ChangeSpriteSheet(cheeseSprite, "../Assets/Sprites/Mouse/Mouse.json");
    }
}
