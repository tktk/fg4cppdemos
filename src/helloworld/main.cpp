#include "fg/util/export.h"
#include "fgpp/def/main/main.h"
#include "fg/def/main/main.h"
#include "fg4cpp/main/args.h"

#include <cstdio>

fg::Int fgMain(
    fg::Args &
)
{
    std::printf( "Hello, world!!\n" );

    return 0;
}

//FIXME
FGEXPORT FgInt fgMain(
    FgArgs *    _args
)
{
    fgMain(
        fg::toFgpp( *_args )
    );
}
