# -*- coding: utf-8 -*-

from . import common

from waflib import Utils

import os.path

FG_HEADERS = 'fgheaders'
FG_LIBPATH = 'fglibpath'
_FG_DIR = 'fg'

FGPP_HEADERS = 'fgppheaders'
_FGPP_DIR = 'fgpp'

FG4CPP_HEADERS = 'fg4cppheaders'
FG4CPP_LIBPATH = 'fg4cpplibpath'
_FG4CPP_DIR = 'fg4cpp'

BUILD = 'build'
BUILD_DEBUG = 'debug'
BUILD_RELEASE = 'release'

COMPILER_TYPE = 'compilertype'
COMPILER_TYPE_CLANG = 'g++'
COMPILER_TYPE_MSVC = 'msvc'

LINKER_TYPE = 'linkertype'
LINKER_TYPE_LD = 'ld'
LINKER_TYPE_MSVC = 'msvc'

TYPE = 'type'
_TYPE_STRING = 'string'

DEFAULT = 'default'

_OS_LINUX = 'linux'
_OS_WINDOWS = 'win32'

def _defaultValue( _VALUES ):
    PLATFORM = Utils.unversioned_sys_platform()

    if PLATFORM in _VALUES:
        return _VALUES[ PLATFORM ]

    return None

OPTIONS = {
    FG_HEADERS : {
        TYPE : _TYPE_STRING,
        DEFAULT : os.path.join(
            '..',
            _FG_DIR,
            common.INCLUDE_DIR,
        ),
    },
    FGPP_HEADERS : {
        TYPE : _TYPE_STRING,
        DEFAULT : os.path.join(
            '..',
            _FGPP_DIR,
            common.INCLUDE_DIR,
        ),
    },
    FG4CPP_HEADERS : {
        TYPE : _TYPE_STRING,
        DEFAULT : os.path.join(
            '..',
            _FG4CPP_DIR,
            common.INCLUDE_DIR,
        ),
    },
    FG_LIBPATH : {
        TYPE : _TYPE_STRING,
        DEFAULT : os.path.join(
            '..',
            _FG_DIR,
            common.BUILD_DIR,
        ),
    },
    FG4CPP_LIBPATH : {
        TYPE : _TYPE_STRING,
        DEFAULT : os.path.join(
            '..',
            _FG4CPP_DIR,
            common.BUILD_DIR,
        ),
    },

    BUILD : {
        TYPE : _TYPE_STRING,
        DEFAULT : BUILD_DEBUG,
    },

    COMPILER_TYPE : {
        TYPE : _TYPE_STRING,
        DEFAULT : _defaultValue(
            {
                _OS_LINUX : COMPILER_TYPE_CLANG,
                _OS_WINDOWS : COMPILER_TYPE_MSVC,
            },
        ),
    },
    LINKER_TYPE : {
        TYPE : _TYPE_STRING,
        DEFAULT : _defaultValue(
            {
                _OS_LINUX : LINKER_TYPE_LD,
                _OS_WINDOWS : LINKER_TYPE_MSVC,
            },
        ),
    },
}
