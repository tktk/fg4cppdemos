﻿#include "fg/util/export.h"
#include "fg4cpp/window/closeevent.h"
#include "fg4cpp/window/sizeevent.h"
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

const auto  TITLE = U"window_setsize";

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

fg::WindowEventHandlers * newWindowEventHandlers(
    fg::Bool &                  _ended
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

    fg::setSizeEventHandler(
        eventHandlers
        , [](
            const fg::WindowSizeEvent & _EVENT
        )
        {
            const auto  WIDTH = fg::getWidth( _EVENT );
            const auto  HEIGHT = fg::getHeight( _EVENT );

            std::printf(
                "fg::Window size[ %d x %d ]\n"
                , WIDTH
                , HEIGHT
            );
        }
    );

    fg::setMouseButtonEventHandler(
        eventHandlers
        , [](
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

            if( _pressed != false ) {
                return;
            }

            const auto  WIDTH = _x + 1;
            const auto  HEIGHT = _y + 1;

            std::printf(
                "fg::setSize() [ %d x %d ]\n"
                , WIDTH
                , HEIGHT
            );

            fg::setSize(
                _window
                , WIDTH
                , HEIGHT
            );
        }
    );

    return eventHandlersUnique.release();
}

fg::Window * newWindow(
    fg::Bool &                  _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = fg::unique(
        newWindowEventHandlers(
            _ended
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
    fg::Bool                ended = false;
    std::mutex              mutex;
    std::condition_variable cond;

    auto    windowUnique = fg::unique(
        newWindow(
            ended
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
