#include "gf/util/export.h"
#include "gf4cpp/window/eventhandlers.h"
#include "gf4cpp/window/window.h"
#include "gf4cpp/string/utf32.h"
#include "gfpp/common/memory.h"
#include "gfpp/common/primitives.h"

#include <cstdio>

enum
{
    WIDTH = 640,
    HEIGHT = 480,
};

const auto  TITLE = U"window_settitle";

gf::Window * newWindow(
)
{
    auto    eventHandlersUnique = gf::unique( gf::newWindowEventHandlers() );
    if( eventHandlersUnique.get() == nullptr ) {
        std::printf( "gf::newWindowEventHandlers()が失敗\n" );

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
    auto    windowUnique = gf::unique( newWindow() );
    if( windowUnique.get() == nullptr ) {
        return 1;
    }
    auto &  window = *windowUnique;

    while( true ) {
        GfStringChar    buffer[ 1024 ];
        std::printf( "> " );
        if( std::fgets(
            buffer
            , sizeof( buffer )
            , stdin
        ) == nullptr ) {
            std::printf( "\n" );

            break;
        }

        auto    titleUnique = gf::unique( gf::newUtf32FromString( buffer ) );
        if( titleUnique.get() == nullptr ) {
            std::printf( "gf::newUtf32FromString()が失敗\n" );

            break;
        }
        const auto &    TITLE = *titleUnique;

        gf::setTitle(
            window
            , TITLE
        );
    }

    return 0;
}
