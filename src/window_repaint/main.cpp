#include "fg/util/export.h"
#include "fg4cpp/window/closeevent.h"
#include "fg4cpp/window/paintevent.h"
#include "fg4cpp/window/eventhandlers.h"
#include "fg4cpp/window/window.h"
#include "fg4cpp/string/utf32.h"
#include "fgpp/common/memory.h"
#include "fgpp/common/primitives.h"

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
    const fg::Bool &            _ENDED
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
    fg::Bool &                  _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    std::unique_lock< std::mutex >  lock( _mutex );

    _ended = true;

    _cond.notify_one();
}

void repaint(
    fg::Window &        _window
    , const fg::Bool &  _PRESSED
    , const fg::Int &   _X
    , const fg::Int &   _Y
    , fg::Int &         _startX
    , fg::Int &         _startY
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
        "fg::repaint() [ %d x %d + %d + %d ]\n"
        , WIDTH
        , HEIGHT
        , X
        , Y
    );

    fg::repaint(
        _window
        , X
        , Y
        , WIDTH
        , HEIGHT
    );
}

void repaint(
    fg::Window &        _window
    , const fg::Bool &  _PRESSED
)
{
    if( _PRESSED != false ) {
        return;
    }

    std::printf( "fg::repaint()\n" );

    fg::repaint(
        _window
    );
}

fg::WindowEventHandlers * newWindowEventHandlers(
    fg::Int &                   _startX
    , fg::Int &                 _startY
    , fg::Bool &                _ended
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
        , [](
            const fg::WindowPaintEvent &    _EVENT
        )
        {
            const auto  WIDTH = fg::getWidth( _EVENT );
            const auto  HEIGHT = fg::getHeight( _EVENT );
            const auto  X = fg::getX( _EVENT );
            const auto  Y = fg::getY( _EVENT );

            std::printf(
                "fg::Window paint[ %d x %d + %d + %d ]\n"
                , WIDTH
                , HEIGHT
                , X
                , Y
            );
        }
    );

    fg::setMouseButtonEventHandler(
        eventHandlers
        , [
            &_startX
            , &_startY
        ]
        (
            fg::Window &    _window
            , fg::ULong     _index
            , fg::Bool      _pressed
            , fg::Int       _x
            , fg::Int       _y
        )
        {
            std::printf(
                "fg::Window button[ %llu, %s, %d x %d ]\n"
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

fg::Window * newWindow(
    fg::Int &                   _startX
    , fg::Int &                 _startY
    , fg::Bool &                _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = fg::unique(
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

FGEXPORT fg::Int main(
)
{
    fg::Int     startX;
    fg::Int     startY;

    fg::Bool                ended = false;
    std::mutex              mutex;
    std::condition_variable cond;

    auto    windowUnique = fg::unique(
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
