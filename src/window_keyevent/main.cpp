#include "gf/util/export.h"
#include "gf4cpp/window/eventhandlers.h"
#include "gf4cpp/window/window.h"
#include "gf4cpp/string/utf32.h"
#include "gf4cpp/string/string.h"
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

const auto  TITLE = U"window_keyevent";

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

    gf::setKeyEventHandler(
        eventHandlers
        , [](
            gf::Window &
            , gf::Key               _key
            , const gf::Utf32Char * _charPtr
            , gf::Bool              _pressed
        )
        {
            gf::Unique< gf::String >::type  charStringUnique;
            if( _charPtr != nullptr ) {
                charStringUnique.reset(
                    gf::newStringFromUnicode(
                        _charPtr
                        , 1
                    )
                );

                if( charStringUnique.get() == nullptr ) {
                    std::printf( "gf::newStringFromUnicode()が失敗\n" );

                    return;
                }
            }

            std::printf(
                "gf::Window key[ 0x%x, '%s', %s ]\n"
                , _key
                , charStringUnique.get() != nullptr
                    ? gf::getPtr( *charStringUnique )
                    : ""
                , _pressed
                    ? "press"
                    : "release"
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
