// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "AudioSystem.h"
#include "Math.h"

class Game
{
public:
    static const int LEVEL_WIDTH = 215;
    static const int LEVEL_HEIGHT = 20;
    static const int TILE_SIZE = 32;
    static const int SPAWN_DISTANCE = 700;
    static const int TRANSITION_TIME = 1;

    enum class GameScene
    {
        MainMenu,
        Intro,
        Level1,
        Level2,
        Level3
    };

    enum class SceneManagerState
    {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState
    {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving
    };

    Game(int windowWidth, int windowHeight);

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    void ProcessInputActors();
    void HandleKeyPressActors(const int key, const bool isPressed);

    // Level functions
    void LoadMainMenu();
    void LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight);

    std::vector<Actor *> GetNearbyActors(const Vector2& position, const int range = 1);
    std::vector<class AABBColliderComponent *> GetNearbyColliders(const Vector2& position, const int range = 2);

    void Reinsert(Actor* actor);

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Audio functions
    class AudioSystem* GetAudio() { return mAudio; }

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen);}
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture* LoadTexture(const std::string& texturePath);

    void SetGameScene(GameScene scene, float transitionTime = .0f);
    void ResetGameScene(float transitionTime = .0f);
    void UnloadScene();

    void SetBackgroundImage(const std::string& imagePath, const Vector2 &position = Vector2::Zero, const Vector2& size = Vector2::Zero);
    void TogglePause();

    std::vector<GameScene> GetGameSceneSequence() {
        return mGameSceneSequence;
    }

    GameScene GetGameScene() {
        return mGameScene;
    }

    // Game-specific
    const class Mouse* GetPlayer1() { return mPlayer1; }
    const class Mouse* GetPlayer2() { return mPlayer2; }

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    void HandleSpell(SDL_Event event);
    int PlayersLeaving();

    int AlivePlayers() {
        int alivePlayers = 0;
        alivePlayers += mPlayer1 ? 1 : 0;
        alivePlayers += mPlayer2 ? 1 : 0;
        return alivePlayers;
    }

private:
    void ProcessInput();
    void UpdateGame();
    void UpdateCamera();
    void GenerateOutput();

    // Scene Manager
    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;

    // HUD functions
    void UpdateLevelTime(float deltaTime);

    // Load the level from a CSV file as a 2D array
    int **ReadLevelData(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);

    // Spatial Hashing for collision detection
    class SpatialHashing* mSpatialHashing;

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    AudioSystem* mAudio;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track actors state
    bool mIsRunning;
    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;
    std::vector<GameScene> mGameSceneSequence;

    // Background and camera
    Vector3 mBackgroundColor;
    Vector3 mModColor;
    Vector2 mCameraPos;

    // Game-specific
    class Mouse *mPlayer1;
    class Mouse *mPlayer2;
    class HUD *mHUD;
    SoundHandle mMusicHandle;

    int mPlayersLeaving;

    float mGameTimer;
    int mGameTimeLimit;

    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundSize;
    Vector2 mBackgroundPosition;

    bool mIsTwoPlayerMode = false;

    void LoadIntroScreen();
    float mIntroTimer = 0.0f;
};
