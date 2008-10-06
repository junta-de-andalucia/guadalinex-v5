# -*- coding: utf-8 -*-
# Copyright (C) 2006-2007  Vodafone España, S.A.
# Author:  Pablo Martí & Rafael Treviño
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"""Images frequently used"""

__version__ = "$Rev: 1172 $"

from os.path import join, exists

from PyQt4.QtGui import QIcon

from vmc.common.consts import IMAGES_DIR

MOBILE_IMG = QIcon(join(IMAGES_DIR, 'mobile.png'))
COMPUTER_IMG = QIcon(join(IMAGES_DIR, 'computer.png'))
THROBBER = QIcon(join(IMAGES_DIR, 'throbber.gif'))

def get_pixbuf_for_device(device):
    """Returns the pixbuf correspondent to C{device}"""
    prop_name = '-'.join(device.name.lower().split(' ')) + '.png'
    prop_path = join(IMAGES_DIR, prop_name)
    if not exists(prop_path):
        prop_path = join(IMAGES_DIR, 'missing.png')
    
    return QIcon(prop_path)