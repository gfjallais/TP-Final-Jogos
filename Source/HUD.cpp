//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // Initialize HUD elements
//    mTimeText = AddText("400", Vector2(mGame->GetWindowWidth() - 150.0f + CHAR_WIDTH, HUD_POS_Y + WORD_OFFSET), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);

//    AddText("World", Vector2(mGame->GetWindowWidth() - 300.0f, HUD_POS_Y), Vector2(CHAR_WIDTH * 5, WORD_HEIGHT), POINT_SIZE);
//    mLevelName = AddText("1-1", Vector2(mGame->GetWindowWidth() - 300.0f + CHAR_WIDTH, HUD_POS_Y + WORD_OFFSET), Vector2(CHAR_WIDTH * 3, WORD_HEIGHT), POINT_SIZE);

//    AddText("Mario", Vector2(40.0f, HUD_POS_Y), Vector2(CHAR_WIDTH * 5, WORD_HEIGHT), POINT_SIZE);
//    mScoreCounter = AddText("000000", Vector2(40.0f, HUD_POS_Y + WORD_OFFSET), Vector2(CHAR_WIDTH * 6, WORD_HEIGHT), POINT_SIZE);
}

HUD::~HUD()
{

}

void HUD::SetLevelName(const std::string &levelName)
{
    mLevelName->SetText(levelName);
}
