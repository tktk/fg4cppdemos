# -*- coding: utf-8 -*-

from . import helloworld

from . import window_closeevent
from . import window_paintevent
from . import window_positionevent
from . import window_sizeevent
from . import window_mousebuttonevent
from . import window_mousemotionevent
from . import window_closerequest
from . import window_settitle
from . import window_setposition
from . import window_setsize
from . import window_repaint

def build( _context ):
    helloworld.build( _context )

    window_closeevent.build( _context )
    window_paintevent.build( _context )
    window_positionevent.build( _context )
    window_sizeevent.build( _context )
    window_mousebuttonevent.build( _context )
    window_mousemotionevent.build( _context )
    window_closerequest.build( _context )
    window_settitle.build( _context )
    window_setposition.build( _context )
    window_setsize.build( _context )
    window_repaint.build( _context )
