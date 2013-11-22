# -*- coding: utf-8 -*-

from wscripts import common
from wscripts import cmdoption
from wscripts import demos

import os.path

APPNAME = 'gf4cppdemos'
VERSION = '0.0.0'

out = common.BUILD_DIR

def _generateFlagsBases(
    common = [],
    debug = [],
    release = [],
):
    return {
        BUILD : common + OPTIONS
        for BUILD, OPTIONS in {
            cmdoption.BUILD_DEBUG : debug,
            cmdoption.BUILD_RELEASE : release,
        }.items()
    }

_CXXS = {
    common.OS_LINUX : cmdoption.CXX_CLANGXX,
    common.OS_WINDOWS : cmdoption.CXX_MSVC,
}

_CXXFLAGS_BASES = {
    cmdoption.CXX_CLANGXX : _generateFlagsBases(
        common = [
            '-Wall',
            '-fno-rtti',
            '-fvisibility=hidden',
            '-std=c++0x',
        ],
        debug = [
            '-O0',
            '-g',
        ],
        release = [
            '-O2',
        ],
    ),
    cmdoption.CXX_MSVC : _generateFlagsBases(
        common = [
            '/Wall',
            '/nologo',
            '/EHs',
        ],
        debug = [
            '/MDd',
            '/Od',
        ],
        release = [
            '/MD',
            '/O2',
            '/Oi',
            '/GL',
        ],
    ),
}

_LINKFLAGS_BASES = {
    cmdoption.CXX_CLANGXX : _generateFlagsBases(
        debug = [
            '-rdynamic',
        ],
    ),
    cmdoption.CXX_MSVC : _generateFlagsBases(
        common = [
            '/NOLOGO',
            '/DYNAMICBASE',
            '/NXCOMPAT',
        ],
        release = [
            '/OPT:REF',
            '/OPT:ICF',
            '/LTCG',
        ],
    ),
}

_DDEBUG = 'DEBUG'
_DOS_LINUX = 'OS_LINUX'
_DOS_WINDOWS = 'OS_WINDOWS'

_DEFINES = {
    common.OS_LINUX : _DOS_LINUX,
    common.OS_WINDOWS : _DOS_WINDOWS,
}

def options( _context ):
    for KEY, DEFAULT in cmdoption.OPTIONS.items():
        _context.add_option(
            _optionKey( KEY ),
            type = DEFAULT[ cmdoption.TYPE ],
            default = DEFAULT[ cmdoption.VALUE ],
        )

    _context.load( 'compiler_cxx' )
    _context.load( 'msvc' )

def _optionKey(
    _KEY
):
    return '--' + _KEY

def configure( _context ):
    _context.msg(
        cmdoption.GF_HEADERS,
        _context.options.gfheaders,
    )
    _context.msg(
        cmdoption.GFPP_HEADERS,
        _context.options.gfppheaders,
    )
    _context.msg(
        cmdoption.GF4CPP_HEADERS,
        _context.options.gf4cppheaders,
    )
    _context.msg(
        cmdoption.GF_LIBPATH,
        _context.options.gflibpath,
    )
    _context.msg(
        cmdoption.GF4CPP_LIBPATH,
        _context.options.gf4cpplibpath,
    )
    _context.msg(
        cmdoption.OS,
        _context.options.os,
    )
    _context.msg(
        cmdoption.BUILD,
        _context.options.build,
    )

    _checkBuild(
        _context,
    )
    _setOsToEnv(
        _context,
    )

    _configureIncludes( _context )
    _configureLibpath( _context )

    _configureDefines( _context )

    flagsBase = _configureCxx( _context )
    flagsBase = _getFlagsBase(
        _context,
        flagsBase,
    )
    _configureCxxflags(
        _context,
        flagsBase,
    )
    _configureLinkflags(
        _context,
        flagsBase,
    )

    _context.load( 'compiler_cxx' )

def _checkBuild( _context ):
    BUILD = _context.options.build

    if BUILD == cmdoption.BUILD_DEBUG:
        return
    if BUILD == cmdoption.BUILD_RELEASE:
        return

    _context.fatal( '非対応のビルドタイプ' )

def _setOsToEnv( _context ):
    _context.env[ cmdoption.OS ] = _context.options.os

def _configureIncludes( _context ):
    includes = [
        os.path.abspath( i )
        for i in [
            common.INCLUDE_DIR,
            _context.options.gfheaders,
            _context.options.gfppheaders,
            _context.options.gf4cppheaders,
        ]
    ]

    _context.msg(
        'includes',
        includes,
    )

    _context.env.INCLUDES = includes

def _configureLibpath( _context ):
    libpath = [
        os.path.abspath( i )
        for i in [
            _context.options.gflibpath,
            _context.options.gf4cpplibpath,
        ]
    ]

    _context.msg(
        'libpath',
        libpath,
    )

def _configureDefines( _context ):
    defines = []

    OS = _context.options.os
    if OS in _DEFINES:
        defines.append( _DEFINES[ OS ] )

    if _context.options.build == cmdoption.BUILD_DEBUG:
        defines.append( _DDEBUG )

    _context.msg(
        'defines',
        defines,
    )

    if len( defines ) > 0:
        _context.env.DEFINES = defines

def _configureCxx( _context ):
    cxx = None

    if _context.options.cxx is not None:
        cxx = _context.options.cxx
    else:
        OS = _context.options.os
        if OS in _CXXS:
            cxx = _CXXS[ OS ]

    if cxx is not None:
        _context.env.CXX = cxx

    return cxx

def _getFlagsBase(
    _context,
    _FLAGS_BASE,
):
    if _context.options.flagsbase is not None:
        return _context.options.flagsbase
    else:
        return _FLAGS_BASE

def _configureCxxflags(
    _context,
    _FLAGS_BASE,
):
    flags = _configureFlags(
        _context,
        cmdoption.CXXFLAGS,
        _FLAGS_BASE,
        _CXXFLAGS_BASES,
        _context.options.cxxflags,
    )

    if flags is not None:
        _context.env.CXXFLAGS = flags

def _configureLinkflags(
    _context,
    _FLAGS_BASE,
):
    flags = _configureFlags(
        _context,
        cmdoption.LINKFLAGS,
        _FLAGS_BASE,
        _LINKFLAGS_BASES,
        _context.options.linkflags,
    )

    if flags is not None:
        _context.env.LINKFLAGS = flags

def _configureFlags(
    _context,
    _FLAGS_NAME,
    _FLAGS_BASE,
    _FLAGS_BASES,
    _FLAGS,
):
    flags = []
    if _FLAGS_BASE in _FLAGS_BASES:
        flags = _FLAGS_BASES[ _FLAGS_BASE ][ _context.options.build ]

    if _FLAGS is not None:
        flags.append( _FLAGS )

    _context.msg(
        _FLAGS_NAME,
        flags,
    )

    if len( flags ) > 0:
        return flags

    return None

def build( _context ):
    demos.build( _context )
