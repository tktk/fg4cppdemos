#include "fg/util/export.h"
#include "fg4cpp/main/args.h"
#include "fg4cpp/gl/config.h"
#include "fg4cpp/gl/gl.h"
#include "fg4cpp/window/closeevent.h"
#include "fg4cpp/window/paintevent.h"
#include "fg4cpp/window/eventhandlers.h"
#include "fg4cpp/window/window.h"
#include "fg4cpp/string/utf32.h"
#include "fgpp/common/memory.h"
#include "fgpp/common/primitives.h"

#include <cstdio>
#include <mutex>
#include <condition_variable>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"gl_simple";

void notifyEnd(
    fg::Bool &                  _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _ended = true;

    _cond.notify_one();
}

void notifyPaint(
    fg::Bool &                  _existsInvalidateRect
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _existsInvalidateRect = true;

    _cond.notify_one();
}

fg::Bool waitPaint(
    const fg::Bool &            _ENDED
    , fg::Bool &                _existsInvalidateRect
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _cond.wait(
        lock
        , [
            &_ENDED
            , &_existsInvalidateRect
        ]
        {
            return _ENDED || _existsInvalidateRect;
        }
    );

    _existsInvalidateRect = false;

    return _ENDED == false;
}

fg::WindowEventHandlers * newWindowEventHandlers(
    fg::Bool &                  _ended
    , fg::Bool &                _existsInvalidateRect
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
            &_ended
            , &_mutex
            , &_cond
        ]
        (
            const fg::WindowCloseEvent &
        )
        {
            notifyEnd(
                _ended
                , _mutex
                , _cond
            );
        }
    );

    fg::setPaintEventHandler(
        eventHandlers
        , [
            &_existsInvalidateRect
            , &_mutex
            , &_cond
        ]
        (
            const fg::WindowPaintEvent &
        )
        {
            notifyPaint(
                _existsInvalidateRect
                , _mutex
                , _cond
            );
        }
    );

    return eventHandlersUnique.release();
}

fg::Window * newWindow(
    fg::Bool &                  _ended
    , fg::Bool &                _existsInvalidateRect
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = fg::unique(
        newWindowEventHandlers(
            _ended
            , _existsInvalidateRect
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

void paintInit(
    fg::GLCurrent & _current
)
{
    fg::glClearColor(
        _current
        , 1
        , 0
        , 0
        , 1
    );
}

void paintLoop(
    fg::GLCurrent & _current
)
{
    std::printf( "paint\n" );

    fg::glClear(
        _current
        , fg::GL_COLOR_BUFFER_BIT
    );

    fg::swapBuffers( _current );
}

fg::Bool paint(
    const fg::GLConfig &        _GL_CONFIG
    , fg::Window &              _window
    , const fg::Bool &          _ENDED
    , fg::Bool &                _existsInvalidateRect
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
        std::printf( "fg::newGLContext()が失敗\n" );

        return false;
    }
    auto &  context = *contextUnique;

    auto    currentUnique = fg::unique(
        fg::getOrNewGLCurrent(
            context
            , _window
        )
    );
    if( currentUnique.get() == nullptr ) {
        std::printf( "fg::getOrNewGLCurrent()が失敗\n" );

        return false;
    }
    auto &  current = *currentUnique;

    paintInit(
        current
    );

    while( true ) {
        if( waitPaint(
            _ENDED
            , _existsInvalidateRect
            , _mutex
            , _cond
        ) == false ) {
            break;
        }

        paintLoop(
            current
        );
    }

    return true;
}

fg::Int fgMain(
    const fg::Args &    _ARGS
)
{
    fg::Bool    ended = false;
    fg::Bool    existsInvalidateRect = true;

    std::mutex              mutex;
    std::condition_variable cond;

    auto    windowUnique = fg::unique(
        newWindow(
            ended
            , existsInvalidateRect
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
        std::printf( "fg::getGLConfig()が失敗\n" );

        return 1;
    }

    if( paint(
        *GL_CONFIG
        , window
        , ended
        , existsInvalidateRect
        , mutex
        , cond
    ) == false ) {
        return 1;
    }

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
