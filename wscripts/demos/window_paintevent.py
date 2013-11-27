# -*- coding: utf-8 -*-

from .. import common
from ..builder import buildShlib

TARGET = 'window_paintevent'

def build( _context ):
    sources = {
        common.SOURCE_DIR : {
            TARGET : {
                'main.cpp',
            },
        },
    }

    libraries = {
        'fg4cpp-window',
        'fg-string',
    }

    buildShlib(
        _context,
        TARGET,
        sources = sources,
        libraries = libraries,
    )
