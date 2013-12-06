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

OS = 'os'
OS_LINUX = 'linux'
OS_WINDOWS = 'windows'

BUILD = 'build'
BUILD_DEBUG = 'debug'
BUILD_RELEASE = 'release'

CXXFLAGS_BASE = 'cxxflagsbase'
CXXFLAGS_BASE_GXX = 'g++'
CXXFLAGS_BASE_MSVC = 'msvc'

LINKFLAGS_BASE = 'linkflagsbase'
LINKFLAGS_BASE_LD = 'ld'
LINKFLAGS_BASE_MSVC = 'msvc'

TYPE = 'type'
_TYPE_STRING = 'string'

DEFAULT = 'default'

def _defaultOs():
    PLATFORM = Utils.unversioned_sys_platform()

    if PLATFORM == 'linux':
        return OS_LINUX
    elif PLATFORM == 'win32':
        return OS_WINDOWS

    return None

def _defaultValue( _VALUES ):
    OS = _defaultOs()

    if OS in _VALUES:
        return _VALUES[ OS ]

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

    OS : {
        TYPE : _TYPE_STRING,
        DEFAULT : _defaultOs(),
    },
    BUILD : {
        TYPE : _TYPE_STRING,
        DEFAULT : BUILD_DEBUG,
    },
    CXXFLAGS_BASE : {
        TYPE : _TYPE_STRING,
        DEFAULT : _defaultValue(
            {
                OS_LINUX : CXXFLAGS_BASE_GXX,
                OS_WINDOWS : CXXFLAGS_BASE_MSVC,
            },
        ),
    },
    LINKFLAGS_BASE : {
        TYPE : _TYPE_STRING,
        DEFAULT : _defaultValue(
            {
                OS_LINUX : LINKFLAGS_BASE_LD,
                OS_WINDOWS : LINKFLAGS_BASE_MSVC,
            },
        ),
    },
}
