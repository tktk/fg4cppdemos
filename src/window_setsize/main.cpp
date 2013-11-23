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
    gf::Bool &                  _ended
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

    gf::setMouseButtonEventHandler(
        eventHandlers
        , [](
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

            if( _pressed != false ) {
                return;
            }

            const auto  WIDTH = _x + 1;
            const auto  HEIGHT = _y + 1;

            std::printf(
                "gf::setSize() [ %d x %d ]\n"
                , WIDTH
                , HEIGHT
            );

            gf::setSize(
                _window
                , WIDTH
                , HEIGHT
            );
        }
    );

    return eventHandlersUnique.release();
}

gf::Window * newWindow(
    gf::Bool &                  _ended
    , std::mutex &              _mutex
    , std::condition_variable & _cond
)
{
    auto    eventHandlersUnique = gf::unique(
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
    gf::Bool                ended = false;
    std::mutex              mutex;
    std::condition_variable cond;

    auto    windowUnique = gf::unique(
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
