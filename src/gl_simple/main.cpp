#include "fg/util/export.h"
#include "fg4cpp/main/args.h"
#include "fg4cpp/gl/config.h"
#include "fg4cpp/gl/gl.h"
#include "fg4cpp/window/eventhandlers.h"
#include "fg4cpp/window/window.h"
#include "fg4cpp/string/utf32.h"
#include "fgpp/common/memory.h"
#include "fgpp/common/primitives.h"

#include <cstdio>
#include <mutex>
#include <condition_variable>
#include <utility>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"gl_simple";

struct Rect
{
    fg::Int x;
    fg::Int y;
    fg::Int width;
    fg::Int height;

    Rect(
    )
    {
        this->reset();
    }

    Rect & operator=(
        const Rect &
    ) = default;

    Rect & operator=(
        Rect && _rect
    )
    {
        *this = _rect;
        _rect.reset();

        return *this;
    }

    void reset(
    )
    {
        this->width = 0;
        this->height = 0;
    }
};

fg::Bool moveRect(
    Rect &                      _rect
    , Rect &                    _newRect
    , const fg::Bool &          _RUNNING
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    if( _newRect.width > 0 && _newRect.height > 0 ) {
    } else if( _RUNNING ) {
        _cond.wait( lock );
    }

    _rect = std::move( _newRect );

    return _RUNNING;
}

void updateRange(
    fg::Int &   _start
    , fg::Int & _volume
    , fg::Int   _newStart
    , fg::Int   _newVolume
)
{
    const auto  END = _start + _volume;
    const auto  NEW_END = _newStart + _newVolume;

    if( _newStart < _start ) {
        _start = _newStart;
        _volume = END - _start;
    }

    if( NEW_END > END ) {
        _volume = NEW_END - _start;
    }
}

void notifyEnd(
    fg::Bool &                  _running
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _running = false;

    _cond.notify_one();
}

void notifyPaint(
    Rect &                      _rect
    , std::mutex &              _mutex
    , std::condition_variable & _cond
    , fg::Int                   _paintX
    , fg::Int                   _paintY
    , fg::Int                   _paintWidth
    , fg::Int                   _paintHeight
)
{
    auto &  x = _rect.x;
    auto &  y = _rect.y;
    auto &  width = _rect.width;
    auto &  height = _rect.height;

    std::unique_lock< std::mutex >  lock( _mutex );

    if( width == 0 || height == 0 ) {
        width = _paintWidth;
        height = _paintHeight;
        x = _paintX;
        y = _paintY;
    } else {
        updateRange(
            x
            , width
            , _paintX
            , _paintWidth
        );

        updateRange(
            y
            , height
            , _paintY
            , _paintHeight
        );
    }

    _cond.notify_one();
}

fg::WindowEventHandlers * newWindowEventHandlers(
    Rect &                      _rect
    , fg::Bool &                _running
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = fg::unique( fg::newWindowEventHandlers() );
    if( eventHandlersUnique.get() == nullptr ) {
        std::printf( "fg::newWindowEventHandlers()が失敗\n" );

        return nullptr;
    }
    auto &  eventHandlers = *eventHandlersUnique;

    fg::setCloseEventHandler(
        eventHandlers
        , [
            &_running
            , &_mutex
            , &_cond
        ]
        (
            fg::Window &
        )
        {
            notifyEnd(
                _running
                , _mutex
                , _cond
            );
        }
    );

    fg::setPaintEventHandler(
        eventHandlers
        , [
            &_rect
            , &_mutex
            , &_cond
        ]
        (
            fg::Window &    _window
            , fg::Int       _paintX
            , fg::Int       _paintY
            , fg::Int       _paintWidth
            , fg::Int       _paintHeight
        )
        {
            notifyPaint(
                _rect
                , _mutex
                , _cond
                , _paintX
                , _paintY
                , _paintWidth
                , _paintHeight
            );
        }
    );

    return eventHandlersUnique.release();
}

fg::Window * newWindow(
    Rect &                      _rect
    , fg::Bool &                _running
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = fg::unique(
        newWindowEventHandlers(
            _rect
            , _running
            , _mutex
            , _cond
        )
    );
    if( eventHandlersUnique.get() == nullptr ) {
        return nullptr;
    }
    auto &  eventHandlers = *eventHandlersUnique;

    auto    titleUnique = fg::unique(
        fg::newUtf32( TITLE )
    );
    if( titleUnique.get() == nullptr ) {
        std::printf( "fg::newUtf32()が失敗\n" );

        return nullptr;
    }
    auto &  title = *titleUnique;

    auto    windowUnique = fg::unique(
        fg::newWindow(
            eventHandlers
            , title
            , WIDTH
            , HEIGHT
        )
    );
    if( windowUnique.get() == nullptr ) {
        std::printf( "fg::newWindow()が失敗\n" );

        return nullptr;
    }

    return windowUnique.release();
}

void paint(
    fg::GLCurrent & _current
    , const Rect &  _rect
)
{
    //TODO
    std::printf( "paint\n" );
    fg::swapBuffers( _current );
}

void paintLoop(
    const fg::GLConfig &        _GL_CONFIG
    , fg::Window &              _window
    , Rect &                    _rect
    , const fg::Bool &          _RUNNING
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    contextUnique = fg::unique(
        fg::newGLContext(
            _GL_CONFIG
        )
    );
    if( contextUnique.get() == nullptr ) {
        return;
    }
    auto &  context = *contextUnique;

    Rect    rect;

    auto    currentUnique = fg::unique(
        fg::getOrNewGLCurrent(
            context
            , _window
        )
    );
    if( currentUnique.get() == nullptr ) {
        return;
    }
    auto &  current = *currentUnique;
        fg::getOrNewGLCurrent(
            context
            , _window
        );

    while( true ) {
        if( moveRect(
            rect
            , _rect
            , _RUNNING
            , _mutex
            , _cond
        ) == false ) {
            break;
        }

        paint(
            current
            , rect
        );
    }
}

fg::Int fgMain(
    const fg::Args &    _ARGS
)
{
    Rect    rect;

    fg::Bool    running = true;

    std::mutex              mutex;
    std::condition_variable cond;

    auto    windowUnique = fg::unique(
        newWindow(
            rect
            , running
            , mutex
            , cond
        )
    );
    if( windowUnique.get() == nullptr ) {
        return 1;
    }
    auto &  window = *windowUnique;

    const auto  GL_CONFIG = fg::getGLConfig( _ARGS );
    if( GL_CONFIG == nullptr ) {
        return 1;
    }

    paintLoop(
        *GL_CONFIG
        , window
        , rect
        , running
        , mutex
        , cond
    );

    return 0;
}

FGEXPORT fg::Int fgMain(
    const FgArgs *  _ARGS
)
{
    return fgMain(
        fg::toFgpp( *_ARGS )
    );
}
