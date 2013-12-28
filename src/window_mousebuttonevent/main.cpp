#include "fg/util/export.h"
#include "fgpp/def/main/main.h"
#include "fg/def/main/main.h"
#include "fg4cpp/def/main/args.h"
#include "fg4cpp/window/mousebuttonevent.h"
#include "fg4cpp/window/eventhandlers.h"
#include "fg4cpp/window/window.h"
#include "fg4cpp/string/utf32.h"
#include "fgpp/common/memory.h"

#include <cstdio>
#include <mutex>
#include <condition_variable>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"window_mousebuttonevent";

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

    fg::setMouseButtonEventHandler(
        eventHandlers
        , [](
            const fg::WindowMouseButtonEvent &  _EVENT
        )
        {
            const auto  INDEX = fg::getIndex( _EVENT );
            const auto  PRESSED = fg::getPressed( _EVENT );
            const auto  X = fg::getX( _EVENT );
            const auto  Y = fg::getY( _EVENT );

            std::printf(
                "fg::Window button[ %llu, %s, %d x %d ]\n"
                , INDEX
                , PRESSED
                    ? "press"
                    : "release"
                , X
                , Y
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

fg::Int fgMain(
    const fg::Args &
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

FGEXPORT FgInt fgMain(
    const FgArgs *  _ARGS
)
{
    return fgMain(
        fg::toFgpp( *_ARGS )
    );
}
