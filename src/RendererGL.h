#ifndef RENDERERGL_H_INCLUDED
#define RENDERERGL_H_INCLUDED

// Onut
#include <onut/Point.h>
#include <onut/Renderer.h>

// Third party
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <SDL.h>

// Forward
#include <onut/ForwardDeclaration.h>
OForwardDeclare(RendererGL);

namespace onut
{
    class RendererGL final : public Renderer
    {
    public:
        RendererGL(const OWindowRef& pWindow);
        ~RendererGL();

        void clear(const Color& color = {.25f, .5f, 1, 1}) override;
        void clearDepth() override;

        void beginFrame() override;
        void endFrame() override;

        void draw(uint32_t vertexCount) override;
        void drawIndexed(uint32_t indexCount) override;

        Point getTrueResolution() const override;
        void onResize(const Point& newSize) override;

        // For effects
        void setKernelSize(const Vector2& kernelSize) override;
        void setSepia(const Vector3& tone = Vector3(1.40f, 1.10f, 0.90f), // 0 - 2.55
                      float saturation = .25f, // 0 - 1
                      float sepiaAmount = .75f) override; // 0 - 1
        void setCRT(const Vector2& kernelSize) override;
        void setCartoon(const Vector3& tone) override;
        void setVignette(const Vector2& kernelSize, float amount = .5f) override;

        void applyRenderStates() override;
        void init(const OWindowRef& pWindow) override;

    private:
        void createDevice(const OWindowRef& pWindow);
        void createRenderTarget();
        void createRenderStates();
        void createUniforms();

        // Device stuff
        SDL_Window* m_pSDLWindow = nullptr;
        SDL_GLContext m_glContext;

        // Render target
        Point m_resolution;

        // Render states

        // Constant buffers

     //   OTextureRef m_boundTextures[RenderStates::MAX_TEXTURES];
    };
};

#endif
