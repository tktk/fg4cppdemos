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

const auto  TITLE = U"window_setsize";

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
    gf::Int &                   _width
    , gf::Int &                 _height
    , std::mutex &              _mutexForSize
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

    gf::setSizeEventHandler(
        eventHandlers
        , [](
            gf::Window &
            , gf::Int       _width
            , gf::Int       _height
        )
        {
            std::printf(
                "gf::Window size[ %d x %d ]\n"
                , _width
                , _height
            );
        }
    );

    gf::setMouseMotionEventHandler(
        eventHandlers
        , [
            &_width
            , &_height
            , &_mutexForSize
        ]
        (
            gf::Window &    _window
            , gf::Int       _x
            , gf::Int       _y
        )
        {
            printf(
                "gf::Window motion[ %d x %d ]\n"
                , _x
                , _y
            );

            std::unique_lock< std::mutex >  lock( _mutexForSize );

            _width = _x + 1;
            _height = _y + 1;
        }
    );

    gf::setMouseButtonEventHandler(
        eventHandlers
        , [
            &_width
            , &_height
            , &_mutexForSize
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

            gf::Int width;
            gf::Int height;

            {
                std::unique_lock< std::mutex >  lock( _mutexForSize );

                width = _width;
                height = _height;
            }

            std::printf(
                "gf::setSize() [ %d x %d ]\n"
                , width
                , height
            );

            gf::setSize(
                _window
                , width
                , height
            );
        }
    );

    return eventHandlersUnique.release();
}

gf::Window * newWindow(
    gf::Int &                   _width
    , gf::Int &                 _height
    , std::mutex &              _mutexForSize
    , gf::Bool &                _ended
    , std::mutex &              _mutexForEnded
    , std::condition_variable & _condForEnded
)
{
    auto    eventHandlersUnique = gf::unique(
        newWindowEventHandlers(
            _width
            , _height
            , _mutexForSize
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
    gf::Int     width;
    gf::Int     height;
    std::mutex  mutexForSize;

    gf::Bool                ended = false;
    std::mutex              mutexForEnded;
    std::condition_variable condForEnded;

    auto    windowUnique = gf::unique(
        newWindow(
            width
            , height
            , mutexForSize
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
