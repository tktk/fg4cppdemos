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

void setRect(
    const gf::Bool &    _PRESSED
    , const gf::Int &   _X
    , const gf::Int &   _Y
    , gf::Int &         _x1
    , gf::Int &         _y1
    , gf::Int &         _x2
    , gf::Int &         _y2
    , std::mutex &      _mutex
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    if( _PRESSED ) {
        _x1 = _X;
        _y1 = _Y;
    } else {
        _x2 = _X;
        _y2 = _Y;
    }
}

void repaint(
    gf::Window &        _window
    , const gf::Bool &  _PRESSED
    , const gf::Int &   _X
    , const gf::Int &   _Y
    , gf::Int &         _x1
    , gf::Int &         _y1
    , gf::Int &         _x2
    , gf::Int &         _y2
    , std::mutex &      _mutex
)
{
    setRect(
        _PRESSED
        , _X
        , _Y
        , _x1
        , _y1
        , _x2
        , _y2
        , _mutex
    );

    if( _PRESSED ) {
        return;
    }

    const auto  X = _x1 < _x2 ? _x1 : _x2;
    const auto  Y = _y1 < _y2 ? _y1 : _y2;
    const auto  WIDTH = std::abs( _x2 - _x1 ) + 1;
    const auto  HEIGHT = std::abs( _y2 - _y1 ) + 1;

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
    gf::Int &                   _x
    , gf::Int &                 _y
    , gf::Int &                 _x1
    , gf::Int &                 _y1
    , gf::Int &                 _x2
    , gf::Int &                 _y2
    , std::mutex &              _mutexForRect
    , gf::Bool &                _ended
    , std::mutex &              _mutexForEnded
    , std::condition_variable & _condForEnded
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
            , &_mutexForEnded
            , &_condForEnded
        ]
        (
            gf::Window &
        )
        {
            notifyEnd(
                _ended
                , _mutexForEnded
                , _condForEnded
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

    gf::setMouseMotionEventHandler(
        eventHandlers
        , [
            &_x
            , &_y
            , &_mutexForRect
        ]
        (
            gf::Window &    _window
            , gf::Int       _newX
            , gf::Int       _newY
        )
        {
            printf(
                "gf::Window motion[ %d x %d ]\n"
                , _newX
                , _newY
            );

            std::unique_lock< std::mutex >  lock( _mutexForRect );

            _x = _newX;
            _y = _newY;
        }
    );

    gf::setMouseButtonEventHandler(
        eventHandlers
        , [
            &_x
            , &_y
            , &_x1
            , &_y1
            , &_x2
            , &_y2
            , &_mutexForRect
        ]
        (
            gf::Window &    _window
            , gf::ULong     _index
            , gf::Bool      _pressed
        )
        {
            switch( _index ) {
            case 0:
                repaint(
                    _window
                    , _pressed
                    , _x
                    , _y
                    , _x1
                    , _y1
                    , _x2
                    , _y2
                    , _mutexForRect
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
    gf::Int &                   _x
    , gf::Int &                 _y
    , gf::Int &                 _x1
    , gf::Int &                 _y1
    , gf::Int &                 _x2
    , gf::Int &                 _y2
    , std::mutex &              _mutexForRect
    , gf::Bool &                _ended
    , std::mutex &              _mutexForEnded
    , std::condition_variable & _condForEnded
)
{
    auto    eventHandlersUnique = gf::unique(
        newWindowEventHandlers(
            _x
            , _y
            , _x1
            , _y1
            , _x2
            , _y2
            , _mutexForRect
            , _ended
            , _mutexForEnded
            , _condForEnded
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
    gf::Int     x;
    gf::Int     y;
    gf::Int     x1;
    gf::Int     y1;
    gf::Int     x2;
    gf::Int     y2;
    std::mutex  mutexForRect;

    gf::Bool                ended = false;
    std::mutex              mutexForEnded;
    std::condition_variable condForEnded;

    auto    windowUnique = gf::unique(
        newWindow(
            x
            , y
            , x1
            , y1
            , x2
            , y2
            , mutexForRect
            , ended
            , mutexForEnded
            , condForEnded
        )
    );
    if( windowUnique.get() == nullptr ) {
        return 1;
    }

    waitEnd(
        ended
        , mutexForEnded
        , condForEnded
    );

    return 0;
}
