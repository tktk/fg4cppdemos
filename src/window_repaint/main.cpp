#include "gf/util/export.h"
#include "gf4cpp/window/eventhandlers.h"
#include "gf4cpp/window/window.h"
#include "gf4cpp/string/utf32.h"
#include "gfpp/common/memory.h"
#include "gfpp/common/primitives.h"

#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <condition_variable>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"window_repaint";

void waitEnd(
    const gf::Bool &            _ENDED
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _cond.wait(
        lock
        , [
            &_ENDED
        ]
        {
            return _ENDED;
        }
    );
}

void notifyEnd(
    gf::Bool &                  _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _ended = true;

    _cond.notify_one();
}

void repaint(
    gf::Window &        _window
    , const gf::Bool &  _PRESSED
    , const gf::Int &   _X
    , const gf::Int &   _Y
    , gf::Int &         _startX
    , gf::Int &         _startY
)
{
    if( _PRESSED ) {
        _startX = _X;
        _startY = _Y;

        return;
    }

    const auto  X = _X < _startX ? _X : _startX;
    const auto  Y = _Y < _startY ? _Y : _startY;
    const auto  WIDTH = std::abs( _X - _startX ) + 1;
    const auto  HEIGHT = std::abs( _Y - _startY ) + 1;

    std::printf(
        "gf::repaint() [ %d x %d + %d + %d ]\n"
        , WIDTH
        , HEIGHT
        , X
        , Y
    );

    gf::repaint(
        _window
        , X
        , Y
        , WIDTH
        , HEIGHT
    );
}

void repaint(
    gf::Window &        _window
    , const gf::Bool &  _PRESSED
)
{
    if( _PRESSED != false ) {
        return;
    }

    std::printf( "gf::repaint()\n" );

    gf::repaint(
        _window
    );
}

gf::WindowEventHandlers * newWindowEventHandlers(
    gf::Int &                   _startX
    , gf::Int &                 _startY
    , gf::Bool &                _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = gf::unique( gf::newWindowEventHandlers() );
    if( eventHandlersUnique.get() == nullptr ) {
        std::printf( "gf::newWindowEventHandlers()が失敗\n" );

        return nullptr;
    }
    auto &  eventHandlers = *eventHandlersUnique;

    gf::setCloseEventHandler(
        eventHandlers
        , [
            &_ended
            , &_mutex
            , &_cond
        ]
        (
            gf::Window &
        )
        {
            notifyEnd(
                _ended
                , _mutex
                , _cond
            );
        }
    );

    gf::setPaintEventHandler(
        eventHandlers
        , [](
            gf::Window &
            , gf::Int       _x
            , gf::Int       _y
            , gf::Int       _width
            , gf::Int       _height
        )
        {
            std::printf(
                "gf::Window paint[ %d x %d + %d + %d ]\n"
                , _width
                , _height
                , _x
                , _y
            );
        }
    );

    gf::setMouseButtonEventHandler(
        eventHandlers
        , [
            &_startX
            , &_startY
        ]
        (
            gf::Window &    _window
            , gf::ULong     _index
            , gf::Bool      _pressed
            , gf::Int       _x
            , gf::Int       _y
        )
        {
            std::printf(
                "gf::Window button[ %llu, %s, %d x %d ]\n"
                , _index
                , _pressed
                    ? "press"
                    : "release"
                , _x
                , _y
            );

            switch( _index ) {
            case 0:
                repaint(
                    _window
                    , _pressed
                    , _x
                    , _y
                    , _startX
                    , _startY
                );
                break;

            case 1:
                repaint(
                    _window
                    , _pressed
                );
                break;

            default:
                break;
            }
        }
    );

    return eventHandlersUnique.release();
}

gf::Window * newWindow(
    gf::Int &                   _startX
    , gf::Int &                 _startY
    , gf::Bool &                _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = gf::unique(
        newWindowEventHandlers(
            _startX
            , _startY
            , _ended
            , _mutex
            , _cond
        )
    );
    if( eventHandlersUnique.get() == nullptr ) {
        return nullptr;
    }
    auto &  eventHandlers = *eventHandlersUnique;

    auto    titleUnique = gf::unique(
        gf::newUtf32( TITLE )
    );
    if( titleUnique.get() == nullptr ) {
        std::printf( "gf::newUtf32()が失敗\n" );

        return nullptr;
    }
    auto &  title = *titleUnique;

    auto    windowUnique = gf::unique(
        gf::newWindow(
            eventHandlers
            , title
            , WIDTH
            , HEIGHT
        )
    );
    if( windowUnique.get() == nullptr ) {
        std::printf( "gf::newWindow()が失敗\n" );

        return nullptr;
    }

    return windowUnique.release();
}

GFEXPORT gf::Int main(
)
{
    gf::Int     startX;
    gf::Int     startY;

    gf::Bool                ended = false;
    std::mutex              mutex;
    std::condition_variable cond;

    auto    windowUnique = gf::unique(
        newWindow(
            startX
            , startY
            , ended
            , mutex
            , cond
        )
    );
    if( windowUnique.get() == nullptr ) {
        return 1;
    }

    waitEnd(
        ended
        , mutex
        , cond
    );

    return 0;
}
