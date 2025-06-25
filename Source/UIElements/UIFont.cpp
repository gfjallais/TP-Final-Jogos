#include "UIFont.h"
#include <vector>
#include <SDL_image.h>

UIFont::UIFont(SDL_Renderer* renderer)
    :mRenderer(renderer)
{

}

UIFont::~UIFont()
{
    Unload();
}

bool UIFont::Load(const std::string& fileName)
{
    // We support these font sizes
    std::vector<int> fontSizes = {8,  9,  10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
                           34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72};

    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 1.: Percorra a lista de tamanhos de fonte e carregue cada fonte usando TTF_OpenFont.
    //  Se alguma fonte falhar ao carregar, registre um erro e retorne false. Caso contrário,
    //  armazene a fonte carregada no mapa mFontData com o tamanho como chave.
    //  --------------
    for(auto size : fontSizes) {
        TTF_Font* ttfFont = TTF_OpenFont(fileName.c_str(), size);

        if(!ttfFont) {
            SDL_Log("Erro ao carregar fonte %s com tamanho: %d - %s", fileName.c_str(), size, TTF_GetError());
            Unload();
            return false;
        }

        mFontData[size] = ttfFont;
    }

    return true;
}

void UIFont::Unload()
{
    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 2.: Percorra o mapa mFontData e feche cada fonte usando TTF_CloseFont.
    //  Em seguida, limpe o mapa mFontData.
    for(auto& [size, font] : mFontData) {
        if(font) {
            TTF_CloseFont(font);
        }
    }
    mFontData.clear();
}

SDL_Texture* UIFont::RenderText(const std::string& text, const Vector3& color /*= Color::White*/,
                                int pointSize /*= 24*/, unsigned wrapLength /*= 900*/)
{
    if(mRenderer == nullptr)
    {
        SDL_Log("Renderer is null. Can't Render Text!");
        return nullptr;
    }

    SDL_Color sdlColor;

    sdlColor.r = static_cast<Uint8>(color.x * 255);
    sdlColor.g = static_cast<Uint8>(color.y * 255);
    sdlColor.b = static_cast<Uint8>(color.z * 255);
    sdlColor.a = 255;

    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 3.1: Verifique se o tamanho do ponto (pointSize) é suportado, ou seja, se está presente no mapa mFontData.
    //  Se o tamanho do ponto for suportado, use TTF_RenderUTF8_Blended_Wrapped para renderizar o texto em uma superfície
    //  SDL_Surface. Se a superfície for criada com sucesso, continue para o próximo passo. Caso contrário, registre um
    //  erro e retorne nullptr.
    if(mFontData.find(pointSize) == mFontData.end()) {
        SDL_Log("Point size: %d, not supported", pointSize);
        return nullptr;
    }

    SDL_Surface* ttfRender = TTF_RenderUTF8_Blended_Wrapped(mFontData[pointSize], text.c_str(), sdlColor, wrapLength);

    if(!ttfRender) {
        SDL_Log("Font failed to render: %s", TTF_GetError());
        return nullptr;
    }

    //  TODO 3.2: Crie uma textura SDL_Texture a partir da superfície retornada por TTF_RenderUTF8_Blended_Wrapped.
    //   Se a criação da textura falhar, registre um erro e retorne nullptr. Caso contrário, retorne a textura criada.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, ttfRender);

    SDL_FreeSurface(ttfRender);  // Always free the surface after creating texture

    if (!texture)
    {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}