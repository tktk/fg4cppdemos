#include "fg/util/export.h"
#include "fgpp/def/main/main.h"
#include "fg/def/main/main.h"
#include "fg4cpp/def/main/args.h"
#include "fg4cpp/window/keyevent.h"
#include "fg4cpp/window/eventhandlers.h"
#include "fg4cpp/window/window.h"
#include "fg4cpp/string/utf32.h"
#include "fg4cpp/string/string.h"
#include "fgpp/common/memory.h"

#include <cstdio>
#include <mutex>
#include <condition_variable>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"window_keyevent";

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

    fg::setKeyEventHandler(
        eventHandlers
        , [](
            const fg::WindowKeyEvent &  _EVENT
        )
        {
            const auto  CHAR_PTR = fg::getCharPtr( _EVENT );

            fg::Unique< fg::String >::type  charStringUnique;
            if( CHAR_PTR != nullptr ) {
                charStringUnique.reset(
                    fg::newStringFromUnicode(
                        CHAR_PTR
                        , 1
                    )
                );

                if( charStringUnique.get() == nullptr ) {
                    std::printf( "fg::newStringFromUnicode()が失敗\n" );

                    return;
                }
            }

            const auto  KEY = fg::getKey( _EVENT );
            const auto  PRESSED = fg::getPressed( _EVENT );

            std::printf(
                "fg::Window key[ 0x%x, '%s', %s ]\n"
                , KEY
                , charStringUnique.get() != nullptr
                    ? fg::getPtr( *charStringUnique )
                    : ""
                , PRESSED
                    ? "press"
                    : "release"
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
