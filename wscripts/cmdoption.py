# -*- coding: utf-8 -*-

from . import common

import os.path

FG_HEADERS = 'fgheaders'
FG_LIBPATH = 'fglibpath'
_FG_DIR = 'fg'

FGPP_HEADERS = 'fgppheaders'
_FGPP_DIR = 'fgpp'

FG4CPP_HEADERS = 'fg4cppheaders'
FG4CPP_LIBPATH = 'fg4cpplibpath'
_FG4CPP_DIR = 'fg4cpp'

OS = 'os'

BUILD = 'build'
BUILD_DEBUG = 'debug'
BUILD_RELEASE = 'release'

CXX = 'cxx'
CXX_CLANGXX = 'clang++'
CXX_MSVC = 'msvc'

FLAGS_BASE = 'flagsbase'
CXXFLAGS = 'cxxflags'
LINKFLAGS = 'linkflags'

TYPE = 'type'
_TYPE_STRING = 'string'

VALUE = 'value'

OPTIONS = {
    FG_HEADERS : {
        TYPE : _TYPE_STRING,
        VALUE : os.path.join(
            '..',
            _FG_DIR,
            common.INCLUDE_DIR,
        ),
    },
    FGPP_HEADERS : {
        TYPE : _TYPE_STRING,
        VALUE : os.path.join(
            '..',
            _FGPP_DIR,
            common.INCLUDE_DIR,
        ),
    },
    FG4CPP_HEADERS : {
        TYPE : _TYPE_STRING,
        VALUE : os.path.join(
            '..',
            _FG4CPP_DIR,
            common.INCLUDE_DIR,
        ),
    },
    FG_LIBPATH : {
        TYPE : _TYPE_STRING,
        VALUE : os.path.join(
            '..',
            _FG_DIR,
            common.BUILD_DIR,
        ),
    },
    FG4CPP_LIBPATH : {
        TYPE : _TYPE_STRING,
        VALUE : os.path.join(
            '..',
            _FG4CPP_DIR,
            common.BUILD_DIR,
        ),
    },

    OS : {
        TYPE : _TYPE_STRING,
        VALUE : common.OS_LINUX,
    },
    BUILD : {
        TYPE : _TYPE_STRING,
        VALUE : BUILD_DEBUG,
    },

    CXX : {
        TYPE : _TYPE_STRING,
        VALUE : None,
    },
    FLAGS_BASE : {
        TYPE : _TYPE_STRING,
        VALUE : None,
    },
    CXXFLAGS : {
        TYPE : _TYPE_STRING,
        VALUE : None,
    },
    LINKFLAGS : {
        TYPE : _TYPE_STRING,
        VALUE : None,
    },
}
