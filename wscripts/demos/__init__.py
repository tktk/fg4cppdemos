# -*- coding: utf-8 -*-

from . import helloworld

from . import window_close

def build( _context ):
    helloworld.build( _context )

    window_close.build( _context )
