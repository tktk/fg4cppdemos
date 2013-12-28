#include "fg/util/export.h"
#include "fgpp/def/main/main.h"
#include "fg/def/main/main.h"
#include "fg4cpp/def/main/args.h"

#include <cstdio>

fg::Int fgMain(
    const fg::Args &
)
{
    std::printf( "Hello, world!!\n" );

    return 0;
}

//FIXME
FGEXPORT FgInt fgMain(
    const FgArgs *  _ARGS
)
{
    return fgMain(
        fg::toFgpp( *_ARGS )
    );
}
