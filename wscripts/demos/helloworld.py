# -*- coding: utf-8 -*-

from .. import common
from .. import cmdoption
from ..builder import buildShlib

TARGET = 'helloworld'

def build( _context ):
    sources = {
        common.SOURCE_DIR : {
            TARGET : {
                'main.cpp',
            },
        },
    }

    buildShlib(
        _context,
        TARGET,
        sources = sources,
    )
