# -*- coding: utf-8 -*-

from . import helloworld

from . import window_closeevent

def build( _context ):
    helloworld.build( _context )

    window_closeevent.build( _context )
