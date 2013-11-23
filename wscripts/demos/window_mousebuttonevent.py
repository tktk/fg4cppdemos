# -*- coding: utf-8 -*-

from .. import common
from ..builder import buildShlib

TARGET = 'window_mousebuttonevent'

def build( _context ):
    sources = {
        common.SOURCE_DIR : {
            TARGET : {
                'main.cpp',
            },
        },
    }

    libraries = {
        'gf4cpp-window',
        'gf-string',
    }

    buildShlib(
        _context,
        TARGET,
        sources = sources,
        libraries = libraries,
    )
