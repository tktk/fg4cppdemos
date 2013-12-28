#include "fg/util/export.h"
#include "fgpp/def/main/main.h"
#include "fg/def/main/main.h"
#include "fg4cpp/def/main/args.h"
#include "fg4cpp/window/eventhandlers.h"
#include "fg4cpp/window/window.h"
#include "fg4cpp/string/utf32.h"
#include "fgpp/common/memory.h"

#include <cstdio>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"window_settitle";

fg::Window * newWindow(
)
{
    auto    eventHandlersUnique = fg::unique( fg::newWindowEventHandlers() );
    if( eventHandlersUnique.get() == nullptr ) {
        std::printf( "fg::newWindowEventHandlers()が失敗\n" );

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
    auto    windowUnique = fg::unique( newWindow() );
    if( windowUnique.get() == nullptr ) {
        return 1;
    }
    auto &  window = *windowUnique;

    while( true ) {
        fg::StringChar  buffer[ 1024 ];
        std::printf( "> " );
        if( std::fgets(
            buffer
            , sizeof( buffer )
            , stdin
        ) == nullptr ) {
            std::printf( "\n" );

            break;
        }

        auto    titleUnique = fg::unique( fg::newUtf32FromString( buffer ) );
        if( titleUnique.get() == nullptr ) {
            std::printf( "fg::newUtf32FromString()が失敗\n" );

            break;
        }
        const auto &    TITLE = *titleUnique;

        fg::setTitle(
            window
            , TITLE
        );
    }

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
