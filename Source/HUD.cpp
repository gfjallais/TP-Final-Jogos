//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Adicione um texto com a string "Time" no canto superior direito da tela, como no jogo orginal. Note que
    //  a classe HUD tem constantes WORD_HEIGHT, WORD_OFFSET, CHAR_WIDTH, POINT_SIZE e HUD_POS_Y que podem ser usadas
    //  para posicionar e definir o tamanho do texto.
    AddText("Time",
        Vector2(mGame->GetWindowWidth() - (8 * CHAR_WIDTH), HUD_POS_Y),
        Vector2((4 * CHAR_WIDTH), WORD_HEIGHT),
        POINT_SIZE);

    // TODO 2.: Adicione um texto com a string "400" (400 segundos) logo abaixo do texto "Time".
    //  Guarde o ponteiro do texto em um membro chamado mTimeText.
    mTimeText = AddText("400",
                        Vector2(mGame->GetWindowWidth() - (4 * CHAR_WIDTH), HUD_POS_Y + WORD_OFFSET),
                        Vector2((4 * CHAR_WIDTH), WORD_HEIGHT),
                        POINT_SIZE);

    // TODO 3.: Adicione um texto com a string "World" à esquerda do texto "Time", como no jogo original.
    AddText("World",
            Vector2(mGame->GetWindowWidth() - (15 * CHAR_WIDTH), HUD_POS_Y),
            Vector2((5 * CHAR_WIDTH), WORD_HEIGHT),
            POINT_SIZE);

    // TODO 4.: Adicione um texto com a string "1-1" logo abaixo do texto "World".
    mLevelName = AddText("1-1",
            Vector2(mGame->GetWindowWidth() - (14 * CHAR_WIDTH), HUD_POS_Y + WORD_OFFSET),
            Vector2((3 * CHAR_WIDTH), WORD_HEIGHT),
            POINT_SIZE);

    // TODO 5.: Adicione um texto com a string "Mario" no canto superior esquerdo da tela, como no jogo original.
    AddText("Mario",
            Vector2((2 * CHAR_WIDTH), HUD_POS_Y),
            Vector2((5 * CHAR_WIDTH), WORD_HEIGHT),
            POINT_SIZE);

    // TODO 6.: Adicione um texto com a string "000000" logo abaixo do texto "Mario".
    mScoreCounter = AddText("000000",
            Vector2((2 * CHAR_WIDTH), HUD_POS_Y + WORD_OFFSET),
            Vector2((6 * CHAR_WIDTH), WORD_HEIGHT),
            POINT_SIZE);

    AddText("C:",
    Vector2((9 * CHAR_WIDTH), HUD_POS_Y),
    Vector2((2 * CHAR_WIDTH), WORD_HEIGHT),
    POINT_SIZE);

    mCoinsCounter = AddText("00",
        Vector2((11 * CHAR_WIDTH), HUD_POS_Y),
        Vector2((2 * CHAR_WIDTH), WORD_HEIGHT),
        POINT_SIZE);
    int coins = mGame->GetCoinsTotal();
}

HUD::~HUD()
{

}

void HUD::SetTime(int time)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Convert time to string
    std::string timeStr = std::to_string(time);
    mTimeText->SetText(timeStr);

    // TODO 2.: Adjust position and size based on number of digits
    int numDigits = timeStr.length();

    // Get the current position of the time text
    Vector2 currentPos = mTimeText->GetPosition();

    float baseRightAlignX = mGame->GetWindowWidth() - (4 * CHAR_WIDTH);
    float newPosX = baseRightAlignX - (numDigits * CHAR_WIDTH);

    // Set the new position
    mTimeText->SetPosition(Vector2(newPosX, HUD_POS_Y + WORD_OFFSET));

    // Set the size proportional to number of digits
    float newWidth = numDigits * CHAR_WIDTH;
    mTimeText->SetSize(Vector2(newWidth, WORD_HEIGHT));
}

void HUD::SetLevelName(const std::string &levelName)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Utilize o método SetText() do mLevelName para atualizar o texto com o nome do nível.
    mLevelName->SetText(levelName);
}

void HUD::SetScore(int score) {
    int numDigits = 6;
    std::string scoreStr = std::to_string(score);

    while (scoreStr.length() < numDigits) {
        scoreStr.insert(0, "0");
    }

    mScoreCounter->SetText(scoreStr);
}

void HUD::SetCoin(int coin) {
    int numDigits = 2;
    std::string coinStr = std::to_string(coin);

    while (coinStr.length() < numDigits) {
        coinStr.insert(0, "0");
    }

    mCoinsCounter->SetText(coinStr);
}