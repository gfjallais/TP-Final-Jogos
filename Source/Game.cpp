// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"
#include "HUD.h"
#include "SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Mario.h"
#include "Actors/Block.h"
#include "Actors/Cheese.h"
#include "Actors/Exit.h"
#include "UIElements/UIScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/DrawComponents/DrawPolygonComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mPlayer1(nullptr)
        ,mPlayer2(nullptr)
        ,mHUD(nullptr)
        ,mBackgroundColor(0, 0, 0)
        ,mModColor(255, 255, 255)
        ,mCameraPos(Vector2::Zero)
        ,mAudio(nullptr)
        ,mGameTimer(0.0f)
        ,mGameTimeLimit(0)
        ,mSceneManagerTimer(0.0f)
        ,mSceneManagerState(SceneManagerState::None)
        ,mGameScene(GameScene::MainMenu)
        ,mNextScene(GameScene::Level1)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundSize(Vector2::Zero)
        ,mBackgroundPosition(Vector2::Zero)
{
    mGameSceneSequence = {GameScene::MainMenu, GameScene::Level1, GameScene::Level2};
}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("CheeseRun", 0, 0, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Start random number generator
    Random::Init();

    // Initialize game systems
    mAudio = new AudioSystem();

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    // Init all game actors
    SetGameScene(GameScene::MainMenu);

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    // Scene Manager FSM: using if/else instead of switch
    if (mSceneManagerState == SceneManagerState::None)
    {
        if (scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2)
        {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
        }
        else {
            SDL_Log("Invalid game scene: %d", static_cast<int>(scene));
            return;
        }
    }
    else
    {
        SDL_Log("Scene Manager is already in a transition state");
        return;
    }
}

void Game::ResetGameScene(float transitionTime)
{
    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplau state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {
        mAudio->StopAllSounds();
        // Set background color
        mBackgroundColor.Set(107.0f, 140.0f, 255.0f);

        // Set background color
        SetBackgroundImage("../Assets/Sprites/Background.png", Vector2(0,0), Vector2(960,640));

        // Initialize main menu actors
        LoadMainMenu();
    }
    else if (mNextScene == GameScene::Level1)
    {
        // Start Music
        mMusicHandle = mAudio->PlaySound("MusicMain.ogg", true);

        // Set background color
        mBackgroundColor.Set(107.0f, 140.0f, 255.0f);

        // Create HUD
        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");

        // Reset HUD
        mGameTimeLimit = 400;
        mHUD->SetTime(mGameTimeLimit);
//        mHUD->SetLevelName("1-1");

        // Set background color
        SetBackgroundImage("../Assets/Sprites/background0.png", Vector2(0,0), Vector2(960,640));

        // Draw Flag
//        auto flag = new Actor(this);
//        flag->SetPosition(Vector2(LEVEL_WIDTH * TILE_SIZE - (16 * TILE_SIZE) - 16, 3 * TILE_SIZE));
//
//        // Add a flag sprite
//        new DrawSpriteComponent(flag, "../Assets/Sprites/Background_Flag.png", 32.0f, 32.0f, 1);

        // Initialize actors
        LoadLevel("../Assets/Levels/level1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }
    else if (mNextScene == GameScene::Level2)
    {
        // Start Music
//        mMusicHandle = mAudio->PlaySound("MusicUnderground.ogg", true);

        // Set background color
        mBackgroundColor.Set(0.0f, 0.0f, 0.0f);

        // Set mod color
//        mModColor.Set(0.0f, 255.0f, 200.0f);
        SetBackgroundImage("../Assets/Sprites/background2.png", Vector2(0,0), Vector2(960,640));
        // Create HUD
        mHUD = new HUD(this, "../Assets/Fonts/SMB.ttf");

        // Reset HUD
        mGameTimeLimit = 400;
        mHUD->SetTime(mGameTimeLimit);
//        mHUD->SetLevelName("1-2");

        // Initialize actors
        LoadLevel("../Assets/Levels/level2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    // Set new scene
    mGameScene = mNextScene;
}

void Game::LoadMainMenu()
{
    UIScreen* mainMenu = new UIScreen(this, "../Assets/Fonts/SMB.ttf");

    const Vector2 bgImgSize = Vector2(mWindowWidth, mWindowHeight);
    const Vector2 bgPos = Vector2(0, 0);
    mainMenu->AddImage("../Assets/Sprites/BackgroundMainMenu.png", bgPos, bgImgSize);

    const Vector2 titleSize = Vector2(176.0f, 176.0f) * 2.0f;
    const Vector2 titlePos = Vector2(mWindowWidth/2.0f - titleSize.x/2.0f, 50.0f);
    mainMenu->AddImage("../Assets/Sprites/Logo.png", titlePos, titleSize);

    const Vector2 buttonSize = Vector2(200.0f, 40.0f);
    const Vector2 button1Pos = Vector2(mWindowWidth/2.0f - buttonSize.x/2.0f, titlePos.y + titleSize.y + 30.0f);
    const Vector2 button2Pos = Vector2(mWindowWidth/2.0f - buttonSize.x/2.0f, button1Pos.y + buttonSize.y + 5.0f);

    mainMenu->AddButton("Jogar", button1Pos, buttonSize, [this]() {
        SetGameScene(GameScene::Level1);
    });

    // mainMenu->AddButton("2 Players", button2Pos, buttonSize, nullptr);
}

void Game::LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight)
{
    // Load level data
    int **mLevelData = ReadLevelData(levelName, levelWidth, levelHeight);

    if (!mLevelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    BuildLevel(mLevelData, levelWidth, levelHeight);
}

void Game::BuildLevel(int** levelData, int width, int height)
{

    // Const map to convert tile ID to block type
    const std::map<int, const std::string> tileMap = {
            {0, "../Assets/Sprites/Blocks/Grass.png"},
            {1, "../Assets/Sprites/Blocks/BlockC.png"},
            {2, "../Assets/Sprites/Blocks/BlockF.png"},
            {4, "../Assets/Sprites/Blocks/Rock.png"},
            {6, "../Assets/Sprites/Blocks/BlockI.png"},
            {8, "empty"},
            {9, "../Assets/Sprites/Blocks/BlockH.png"},
            {12, "../Assets/Sprites/Blocks/BlockG.png"},
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            int tile = levelData[y][x];

            const float forwardSpeed = 1000.0f;
            const float jumpSpeed = -555.0f;

            if(tile == 16) // Mario
            {
                mPlayer1 = new Mario(this, forwardSpeed, jumpSpeed, true);
                mPlayer1->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                mPlayer2 = new Mario(this, forwardSpeed, jumpSpeed, false);
                mPlayer2->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if(tile == 3)
            {
                Cheese* cheese = new Cheese(this);
                cheese->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if(tile == 13)
            {
                Exit* exit = new Exit(this);
                exit->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else // Blocks
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    // Create a block actor
                    Block* block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

int **Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i) {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof()) {
        std::getline(file, line);

        if(!line.empty()) {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                // Check if the Return key has been pressed to pause/unpause the game
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    TogglePause();
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                HandleSpell(event);
        }
    }

    ProcessInputActors();
}

void Game::HandleSpell(SDL_Event event) {
    SDL_Log("%d", event.button.button);
    if(event.button.button == SDL_BUTTON_LEFT) {
        if(mPlayer1) {
            if(mPlayer1->GetSpellMode()) {
                mPlayer1->CastSpell(event.button.x, event.button.y);
            }
        }
    }
    if(event.button.button == SDL_BUTTON_RIGHT) {
        if(mPlayer2) {
            if(mPlayer2->GetSpellMode()) {
                mPlayer2->CastSpell(event.button.x, event.button.y);
            }
        }
    }
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool arePlayersOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

            if (actor == mPlayer1 || actor == mPlayer2) {
                arePlayersOnCamera = true;
            }
        }

        // If Mario is not on camera, process input for him
        if (!arePlayersOnCamera && (mPlayer1 || mPlayer2)) {
            if(mPlayer1) {
                mPlayer1->ProcessInput(state);
            }
            if(mPlayer2) {
                mPlayer2->ProcessInput(state);
            }
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        // Handle key press for actors
        bool arePlayersOnCamera = false;
        for (auto actor: actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mPlayer1 && actor == mPlayer2) {
                arePlayersOnCamera = true;
            }
        }

        // If Mario is not on camera, handle key press for him
        if (!arePlayersOnCamera && (mPlayer1 || mPlayer2)) {
            if(mPlayer1) {
                mPlayer1->HandleKeyPress(key, isPressed);
            }
            if(mPlayer2) {
                mPlayer2->HandleKeyPress(key, isPressed);
            }
        }
    }

}

void Game::TogglePause()
{
    if (mGameScene != GameScene::MainMenu)
    {
        if (mGamePlayState == GamePlayState::Playing)
        {
            mGamePlayState = GamePlayState::Paused;
            mAudio->PlaySound("Coin.wav");
            mAudio->PauseSound(mMusicHandle);
        }
        else if (mGamePlayState == GamePlayState::Paused)
        {
            mGamePlayState = GamePlayState::Playing;
            mAudio->PlaySound("Coin.wav");
            mAudio->ResumeSound(mMusicHandle);
        }
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if(mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::GameOver)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------
    if(mGameScene != GameScene::MainMenu && mGamePlayState == GamePlayState::Playing)
    {
        // Reinsert level time
        UpdateLevelTime(deltaTime);
    }

    UpdateSceneManager(deltaTime);
    UpdateCamera();
}

void Game::UpdateSceneManager(float deltaTime)
{
    if(mSceneManagerState == SceneManagerState::Entering)
    {
        mSceneManagerTimer -= deltaTime;
        if(mSceneManagerTimer <= 0.0f)
        {
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active;
        }
    }
    else if(mSceneManagerState == SceneManagerState::Active)
    {
        mSceneManagerTimer -= deltaTime;
        if(mSceneManagerTimer <= 0.0f)
        {
            ChangeScene();
            mSceneManagerState = SceneManagerState::None;
        }
    }
}

void Game::UpdateLevelTime(float deltaTime)
{
    // Reinsert game timer
    mGameTimer += deltaTime;
    if (mGameTimer >= 0.5f)
    {
        mGameTimer = 0.0f;
        mGameTimeLimit--;

        if (mGameTimeLimit > 0) {
            mHUD->SetTime(mGameTimeLimit);
        }
        else
        {
            // Kill Mario if time limit is reached
            mHUD->SetTime(mGameTimeLimit);
            mPlayer1->Kill();
            mPlayer2->Kill();
        }
    }
}

void Game::UpdateCamera()
{
    if (!mPlayer1 && !mPlayer2) return;

    float player1PosX = mPlayer1 ? mPlayer1->GetPosition().x : 0.0f;
    float player2PosX = mPlayer2 ? mPlayer2->GetPosition().x : 0.0f;

    float midpoint = (player1PosX + player2PosX) / 2.0f;

    float horizontalCameraPos = midpoint - (mWindowWidth / 2.0f);

    float maxCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
    horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f, maxCameraPos);

    mCameraPos.x = horizontalCameraPos;
}

int Game::PlayersLeaving() {
    int count = 0;
    if(mPlayer1) {
        if(mPlayer1->GetIsLeaving()) count++;
    }
    if(mPlayer2) {
        if(mPlayer2->GetIsLeaving()) count++;
    }
    return count;
}

void Game::UpdateActors(float deltaTime)
{
    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
        mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    bool arePlayersOnCamera = false;
    for (auto actor : actorsOnCamera)
    {
        actor->Update(deltaTime);
        if (actor == mPlayer1 && actor == mPlayer2) {
            arePlayersOnCamera = true;
        }
    }

    // If Mario is not on camera, update him (player should always be updated)
    if (!arePlayersOnCamera && (mPlayer1 || mPlayer2)) {
        if(mPlayer1) {
            mPlayer1->Update(deltaTime);
        }
        if(mPlayer2) {
            mPlayer2->Update(deltaTime);
        }
    }

    for (auto actor : actorsOnCamera)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            delete actor;
            if (actor == mPlayer1) {
                mPlayer1 = nullptr;
            }
            if (actor == mPlayer2) {
                mPlayer2 = nullptr;
            }
        }
    }
}

void Game::AddActor(Actor* actor)
{
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    mSpatialHashing->Remove(actor);
}
void Game::Reinsert(Actor* actor)
{
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor *> Game::GetNearbyActors(const Vector2& position, const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *> Game::GetNearbyColliders(const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y, mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    // Draw background texture considering camera position
    if (mBackgroundTexture)
    {
        SDL_Rect dstRect = { static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
                             static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
                             static_cast<int>(mBackgroundSize.x),
                             static_cast<int>(mBackgroundSize.y) };

        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    // Draw transition rect
    if (mSceneManagerState == SceneManagerState::Active)
    {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
        SDL_Rect rect = { 0, 0, mWindowWidth, mWindowHeight };
        SDL_RenderFillRect(mRenderer, &rect);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2 &position, const Vector2 &size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}


UIFont* Game::LoadFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }
    else
    {
        UIFont* font = new UIFont(mRenderer);
        if (font->Load(fileName))
        {
            mFonts.emplace(fileName, font);
        }
        else
        {
            font->Unload();
            delete font;
            font = nullptr;
        }
        return font;
    }
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
