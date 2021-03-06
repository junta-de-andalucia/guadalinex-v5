# user-settings.py - custom Orca settings
# Generated by orca.  DO NOT EDIT THIS FILE!!!
# If you want permanent customizations that will not
# be overwritten, edit orca-customizations.py.
#
import re
import time

import orca.debug
import orca.settings
import orca.acss

#orca.debug.debugLevel = orca.debug.LEVEL_OFF
orca.debug.debugLevel = orca.debug.LEVEL_SEVERE
#orca.debug.debugLevel = orca.debug.LEVEL_WARNING
#orca.debug.debugLevel = orca.debug.LEVEL_INFO
#orca.debug.debugLevel = orca.debug.LEVEL_CONFIGURATION
#orca.debug.debugLevel = orca.debug.LEVEL_FINE
#orca.debug.debugLevel = orca.debug.LEVEL_FINER
#orca.debug.debugLevel = orca.debug.LEVEL_FINEST
#orca.debug.debugLevel = orca.debug.LEVEL_ALL

#orca.debug.eventDebugLevel = orca.debug.LEVEL_OFF
#orca.debug.eventDebugFilter =  None
#orca.debug.eventDebugFilter = re.compile('[\S]*focus|[\S]*activ')
#orca.debug.eventDebugFilter = re.compile('nomatch')
#orca.debug.eventDebugFilter = re.compile('[\S]*:accessible-name')

#orca.debug.debugFile = open(time.strftime('debug-%Y-%m-%d-%H:%M:%S.out'), 'w', 0)
#orca.debug.debugFile = open('debug.out', 'w', 0)

#orca.settings.useBonoboMain=False
#orca.settings.debugEventQueue=True
#orca.settings.gilSleepTime=0

if False:
    import sys
    import orca.util
    sys.settrace(orca.util.traceit)
    orca.debug.debugLevel = orca.debug.LEVEL_ALL

orca.settings.orcaModifierKeys = ['Insert', 'KP_Insert']
orca.settings.enableSpeech = True
orca.settings.speechServerFactory = 'orca.gnomespeechfactory'
orca.settings.speechServerInfo = ['Festival GNOME Speech Driver', 'OAFIID:GNOME_Speech_SynthesisDriver_Festival:proto0.3']
orca.settings.voices = {
'default' : orca.acss.ACSS({'family': {'name': 'el_diphone'}}),
'uppercase' : orca.acss.ACSS({'average-pitch': 6}),
'hyperlink' : orca.acss.ACSS({'average-pitch': 8}),
}
orca.settings.speechVerbosityLevel = orca.settings.VERBOSITY_LEVEL_VERBOSE
orca.settings.readTableCellRow = True
orca.settings.enableSpeechIndentation = False
orca.settings.enableEchoByWord = False
orca.settings.enableKeyEcho = False
orca.settings.enablePrintableKeys = True
orca.settings.enableModifierKeys = True
orca.settings.enableLockingKeys = True
orca.settings.enableFunctionKeys = True
orca.settings.enableActionKeys = True
orca.settings.enableBraille = True
orca.settings.enableBrailleGrouping = False
orca.settings.brailleVerbosityLevel = orca.settings.VERBOSITY_LEVEL_VERBOSE
orca.settings.brailleRolenameStyle = orca.settings.BRAILLE_ROLENAME_STYLE_LONG
orca.settings.enableBrailleMonitor = False
orca.settings.enableMagnifier = False
orca.settings.enableMagCursor = True
orca.settings.enableMagCursorExplicitSize = False
orca.settings.magCursorSize = 32
orca.settings.magCursorColor = '#000000'
orca.settings.enableMagCrossHair = True
orca.settings.enableMagCrossHairClip = False
orca.settings.magCrossHairSize = 16
orca.settings.magZoomerLeft = 384
orca.settings.magZoomerRight = 758
orca.settings.magZoomerTop = 0
orca.settings.magZoomerBottom = 566
orca.settings.magZoomFactor = 4
orca.settings.enableMagZoomerColorInversion = False
orca.settings.magSmoothingMode = orca.settings.MAG_SMOOTHING_MODE_BILINEAR
orca.settings.magMouseTrackingMode = orca.settings.MAG_MOUSE_TRACKING_MODE_CENTERED
orca.settings.verbalizePunctuationStyle = orca.settings.PUNCTUATION_STYLE_MOST

try:
    __import__("orca-customizations")
except ImportError:
    pass
