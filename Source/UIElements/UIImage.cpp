//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"

UIImage::UIImage(SDL_Renderer* renderer, const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color)
    : UIElement(pos, size, color),
    mTexture(nullptr),
    mRenderer(renderer)
{
    // --------------
    // TODO - PARTE 1-3
    // --------------

    // TODO 1.: Replique o código do método LoadTexture da classe Game, mas desta vez carregue a imagem
    //  a partir do caminho imagePath passado como parâmetro. Armazene o resultado em mTexture.
    SDL_Surface* surface = IMG_Load(imagePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        mTexture = nullptr;
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!mTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
    }
}

UIImage::~UIImage()
{
    // --------------
    // TODO - PARTE 1-3
    // --------------

    // TODO 1.: Libere a textura mTexture, se ela não for nula, utilizando SDL_DestroyTexture.
    //  Não se esqueça de definir mTexture como nullptr após liberar a textura.
    if (mTexture != nullptr)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void UIImage::Draw(SDL_Renderer* renderer, const Vector2 &screenPos)
{
    // --------------
    // TODO - PARTE 1-3
    // --------------

    // TODO 1.: Verifique se mTexture é nula. Se for, retorne imediatamente.
    if(!mTexture) {
        return;
    }

    // TODO 2.: Crie um SDL_Rect para definir a posição e o tamanho da imagem na tela. A posição deve ser
    //  relativa ao screenPos passado como parâmetro, ou seja, some screenPos com mPosition.
    SDL_Rect destRect;
    destRect.x = static_cast<int>(mPosition.x + screenPos.x);
    destRect.y = static_cast<int>(mPosition.y + screenPos.y);
    destRect.w = static_cast<int>(mSize.x);
    destRect.h = static_cast<int>(mSize.y);

    // TODO 3.: Desenhe a textura mTexture no renderer usando SDL_RenderCopy.
    SDL_RenderCopy(renderer, mTexture, nullptr, &destRect);
}