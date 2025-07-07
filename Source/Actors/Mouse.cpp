//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mouse.h"
#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include <algorithm>

Mouse::Mouse(Game* game, const float forwardSpeed, const float jumpSpeed, const bool isPlayer1)
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
    mRigidBodyComponent = new RigidBodyComponent(this, 1.5f, 5.0f);
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
    mDrawComponent->AddAnimation("wizard", {9});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mCollectedCheese = false;
}

bool Mouse::PressedLeft(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_A];
    }
    return state[SDL_SCANCODE_LEFT];
}

bool Mouse::PressedRight(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_D];
    }
    return state[SDL_SCANCODE_RIGHT];
}

bool Mouse::PressedUp(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_W];
    }
    return state[SDL_SCANCODE_UP];
}

bool Mouse::PressedDown(const uint8_t* state) {
    if(mIsPlayer1) {
        return state[SDL_SCANCODE_S];
    }
    return state[SDL_SCANCODE_DOWN];
}

void Mouse::OnProcessInput(const uint8_t* state)
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
            ChangeToWizardSprite(mSpellMode);
            SDL_Log("Toggled spellMode %d", mSpellMode);
        }
    }

    if (!PressedLeft(state) && !PressedRight(state))
    {
        mIsRunning = false;
    }
}

void Mouse::ToggleSpellMode() {
    mSpellMode = !mSpellMode;
    if (!mSpellMode) {
        mShowBlockPreview = false;
    }
}

void Mouse::CastSpell(int x, int y) {
    SDL_Log("Cast Spell Called");
    SDL_Log("%d", mSpellCount);
    if(mSpellCount > 0) {
        Block* block = new Block(mGame, "../Assets/Sprites/Blocks/rock.png", false);
        block->SetPosition(mBlockPreviewPos);
        mSpellCount--;
        ToggleSpellMode();
        ChangeToWizardSprite(mSpellMode);
    }
}

void Mouse::OnHandleKeyPress(const int key, const bool isPressed)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    // Jump
    if (((key == SDLK_w && mIsPlayer1) || (key == SDLK_UP && !mIsPlayer1)) && isPressed && (mIsOnGround || (mCanWallJump && !mIsOnWall)) && !mSpellMode)
    {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
        mCanWallJump = false;
        mWallJumpCooldown = 0;
        mIsOnGround = false;
        // SetSpeed(2000.0f);

        // Play jump sound
//        mGame->GetAudio()->PlaySound("Jump.wav");
    }
}

  void Mouse::OnUpdate(float deltaTime)
{
    // SetSpeed(1000.0f);
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0) {
        mIsOnGround = false;
    }

    if(mWallJumpCooldown > 0) {
        mCanWallJump = true;
        mWallJumpCooldown--;
    } else {
        mCanWallJump = false;
    }

    mPosition.x = Math::Max(mPosition.x, mGame->GetCameraPos().x);

    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing && mPosition.y > mGame->GetWindowHeight()) {
        Kill();
    }


    if (mGame->GetGamePlayState() == Game::GamePlayState::Leaving)
    {
        mState = ActorState::Destroy;
        auto gameScene = mGame->GetGameSceneSequence();
        auto currentIt = std::find(gameScene.begin(), gameScene.end(), mGame->GetGameScene());
        if (currentIt != gameScene.end()) {
            auto nextIt = currentIt + 1;
            if (nextIt != gameScene.end()) {
                mGame->SetGameScene(*nextIt);
            } else {
                mGame->SetGameScene(Game::GameScene::MainMenu);
            }
        }
        return;
    }

    ManageAnimations();
}

void Mouse::ManageAnimations()
{
    if(mIsDying)
    {
        mDrawComponent->SetAnimation("Dead");
    }
    else if(mSpellMode) {
        mDrawComponent->SetAnimation("wizard");
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

void Mouse::Kill()
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

void Mouse::Win()
{
}

void Mouse::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
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

void Mouse::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
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

void Mouse::CollectCheese() {
    if (!mCollectedCheese){
        mCollectedCheese = true;
        mForwardSpeed = 800.0f;
        mJumpSpeed = -525.0f;
        mGame->GetAudio()->PlaySound("cheese.wav");

        std::string cheeseSprite = mIsPlayer1 ? "../Assets/Sprites/Mouse/Mouse1_cheese.png" : "../Assets/Sprites/Mouse/Mouse2_cheese.png";
        mDrawComponent->ChangeSpriteSheet(cheeseSprite, "../Assets/Sprites/Mouse/Mouse.json");
    }
}

void Mouse::ChangeToWizardSprite(bool toWizard) {
    std::string spritePath;
    if (toWizard) {
        if (mIsPlayer1) {
            spritePath = mCollectedCheese ? "../Assets/Sprites/Mouse/Mouse1_cheese_wizard.png" : "../Assets/Sprites/Mouse/Mouse1_wizard.png";
        } else {
            spritePath = mCollectedCheese ? "../Assets/Sprites/Mouse/Mouse2_cheese_wizard.png" : "../Assets/Sprites/Mouse/Mouse2_wizard.png";
        }
    } else {
        if (mIsPlayer1) {
            spritePath = mCollectedCheese ? "../Assets/Sprites/Mouse/Mouse1_cheese.png" : "../Assets/Sprites/Mouse/Mouse1.png";
        } else {
            spritePath = mCollectedCheese ? "../Assets/Sprites/Mouse/Mouse2_cheese.png" : "../Assets/Sprites/Mouse/Mouse2.png";
        }
    }
    mDrawComponent->ChangeSpriteSheet(spritePath, "../Assets/Sprites/Mouse/Mouse.json");

    if(mSpellMode) {
        mDrawComponent->SetAnimation("wizard");
        SDL_Log("Wizard mode");
    } else {
        mDrawComponent->SetAnimation("idle");
        SDL_Log("Idle mode");
    }
}

void Mouse::UpdateBlockPreview(int mouseX, int mouseY) {
    if (!mSpellMode) {
        mShowBlockPreview = false;
        return;
    }
    int gridX = (mouseX / Game::TILE_SIZE) * Game::TILE_SIZE;
    int gridY = (mouseY / Game::TILE_SIZE) * Game::TILE_SIZE;
    mBlockPreviewPos = Vector2(gridX, gridY);
    mShowBlockPreview = true;
}

void Mouse::DrawBlockPreview(SDL_Renderer* renderer) {
    if (!mShowBlockPreview) return;
    SDL_Texture* previewTexture = mGame->LoadTexture("../Assets/Sprites/Blocks/rock.png");
    SDL_Rect dstRect = {
        static_cast<int>(mBlockPreviewPos.x - mGame->GetCameraPos().x),
        static_cast<int>(mBlockPreviewPos.y - mGame->GetCameraPos().y),
        Game::TILE_SIZE,
        Game::TILE_SIZE
    };
    SDL_SetTextureAlphaMod(previewTexture, 128); // 50% transparent
    SDL_RenderCopy(renderer, previewTexture, nullptr, &dstRect);
    SDL_SetTextureAlphaMod(previewTexture, 255); // Reset alpha
    SDL_DestroyTexture(previewTexture);
}
