# -*- coding: utf-8 -*-

from .. import common
from ..builder import buildShlib

TARGET = 'gl_simple'

def build( _context ):
    sources = {
        common.SOURCE_DIR : {
            TARGET : {
                'main.cpp',
            },
        },
    }

    libraries = {
        'fg-main',
        'fg-string',
        'fg4cpp-window',
        'fg-gl',
    }

    buildShlib(
        _context,
        TARGET,
        sources = sources,
        libraries = libraries,
    )
