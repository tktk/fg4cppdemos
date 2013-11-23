#include "gf/util/export.h"
#include "gf4cpp/window/eventhandlers.h"
#include "gf4cpp/window/window.h"
#include "gf4cpp/string/utf32.h"
#include "gfpp/common/memory.h"
#include "gfpp/common/primitives.h"

#include <cstdio>
#include <mutex>
#include <condition_variable>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"window_setposition";

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

gf::WindowEventHandlers * newWindowEventHandlers(
    gf::Int &                   _x
    , gf::Int &                 _y
    , std::mutex &              _mutexForPosition
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

    gf::setMouseMotionEventHandler(
        eventHandlers
        , [
            &_x
            , &_y
            , &_mutexForPosition
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

            std::unique_lock< std::mutex >  lock( _mutexForPosition );

            _x = _newX;
            _y = _newY;
        }
    );

    gf::setMouseButtonEventHandler(
        eventHandlers
        , [
            &_x
            , &_y
            , &_mutexForPosition
        ]
        (
            gf::Window &    _window
            , gf::ULong
            , gf::Bool      _pressed
        )
        {
            if( _pressed != false ) {
                return;
            }

            gf::Int x;
            gf::Int y;

            {
                std::unique_lock< std::mutex >  lock( _mutexForPosition );

                x = _x;
                y = _y;
            }

            std::printf(
                "gf::setPosition() [ %d x %d ]\n"
                , x
                , y
            );

            gf::setPosition(
                _window
                , x
                , y
            );
        }
    );

    return eventHandlersUnique.release();
}

gf::Window * newWindow(
    gf::Int &                   _x
    , gf::Int &                 _y
    , std::mutex &              _mutexForPosition
    , gf::Bool &                _ended
    , std::mutex &              _mutexForEnded
    , std::condition_variable & _condForEnded
)
{
    auto    eventHandlersUnique = gf::unique(
        newWindowEventHandlers(
            _x
            , _y
            , _mutexForPosition
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
    std::mutex  mutexForPosition;

    gf::Bool                ended = false;
    std::mutex              mutexForEnded;
    std::condition_variable condForEnded;

    auto    windowUnique = gf::unique(
        newWindow(
            x
            , y
            , mutexForPosition
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
