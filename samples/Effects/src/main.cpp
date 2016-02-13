// Required for WinMain
#include <Windows.h>

// Oak Nut include
#include "onut_old.h"
#include "onut/Font.h"
#include "onut/Texture.h"

void init();
void update();
void render();

OTextureRef pBlured = nullptr;
OTextureRef pSepia = nullptr;
OTextureRef pCRT = nullptr;
OTextureRef pCartoon = nullptr;
OTextureRef pVignette = nullptr;

// Main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    OSettings->setGameName("Sprites Sample");
    ORun(init, update, render);
}

void init()
{
    pBlured = OTexture::createRenderTarget({256, 256});
    pSepia = OTexture::createRenderTarget({256, 256});
    pCRT = OTexture::createRenderTarget({256, 256});
    pCartoon = OTexture::createRenderTarget({256, 256});
    pVignette = OTexture::createRenderTarget({256, 256});
}

void update()
{
}

void render()
{
    auto pLandscape = OGetTexture("landscape.png");
    auto drawLandscapeToRenderTarget = [=](const OTextureRef& pRenderTarget)
    {
        pRenderTarget->bindRenderTarget();
        OSB->begin();
        OSB->drawRect(pLandscape, {0, 0, pRenderTarget->getSizef().x, pRenderTarget->getSizef().y});
        OSB->end();
        pRenderTarget->unbindRenderTarget();
    };

    // Clear
    ORenderer->clear(OColorHex(1d232d));

    // Draw landscape to our render targets
    drawLandscapeToRenderTarget(pBlured);
    drawLandscapeToRenderTarget(pSepia);
    drawLandscapeToRenderTarget(pCRT);
    drawLandscapeToRenderTarget(pCartoon);
    drawLandscapeToRenderTarget(pVignette);
    
    // Apply effects
    pBlured->blur();
    pSepia->sepia();
    pCRT->crt();
    pCartoon->cartoon();
    pVignette->vignette();

    // Draw out resulted textures
    auto pFont = OGetFont("font.fnt");
    OSpriteBatch->begin();

    OSB->drawRect(pLandscape, {0, 0, 256, 256});
    OSB->drawRect(pBlured, {256, 0, 256, 256});
    OSB->drawRect(pSepia, {512, 0, 256, 256});
    OSB->drawRect(pCRT, {0, 288, 256, 256});
    OSB->drawRect(pCartoon, {256, 288, 256, 256});
    OSB->drawRect(pVignette, {512, 288, 256, 256});

    pFont->draw("Original", {128 + 0, 256 + 8}, OCenter);
    pFont->draw("Blur", {128 + 256, 256 + 8}, OCenter);
    pFont->draw("Sepia", {128 + 512, 256 + 8}, OCenter);
    pFont->draw("CRT", {128 + 0, 288 + 256 + 8}, OCenter);
    pFont->draw("Cartoon", {128 + 256, 288 + 256 + 8}, OCenter);
    pFont->draw("Vignette", {128 + 512, 288 + 256 + 8}, OCenter);

    OSpriteBatch->end();
}
