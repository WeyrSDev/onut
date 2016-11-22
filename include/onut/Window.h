#pragma once
// Onut
#include <onut/Point.h>

// Third party
#if defined(WIN32)
#include <Windows.h>
#endif

// STL
#include <functional>
#include <string>

// Forward
#include <onut/ForwardDeclaration.h>
OForwardDeclare(Window);

namespace onut
{
    class Window
    {
    public:
        static OWindowRef create();

        virtual ~Window();

#if defined(WIN32)
        virtual HWND getHandle() = 0;
        virtual void setCursor(HCURSOR cursor) = 0;
        virtual HCURSOR getCursor() const = 0;
#endif
        virtual void setCaption(const std::string& newName) = 0;

        std::function<void(char)>                   onWrite;
        std::function<void(uintptr_t)>              onKey;
        std::function<void(uint32_t)>               onMenu;
        std::function<void(const std::string&)>     onDrop;
        std::function<void(const Point& newSize)>   onResize;
        std::function<bool()>                       onQuit;

    protected:
        Window();
    };
}

extern OWindowRef oWindow;
