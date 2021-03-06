# -*- coding: utf-8 -*-
# Copyright (C) 2006-2007  Vodafone España, S.A.
# Author:  Pablo Martí
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
"""Unittests for the command module"""

__version__ = "$Rev: 1172 $"

import re
from twisted.trial import unittest

from vmc.common.command import get_cmd_dict_copy

cmd_dict = get_cmd_dict_copy()

class TestCommandsRegexps(unittest.TestCase):
    """Test for csvutils functionality"""
    
    def test_check_pin_regexp(self):
        # [-] SENDING ATCMD 'AT+CPIN?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CPIN: READY\r\n\r\nOK\r\n'
        extract = cmd_dict['check_pin']['extract']
        text = '\r\n+CPIN: READY\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('resp'), 'READY')
        # [-] WAITING: DATA_RCV = '\r\n+CPIN: SIM PIN\r\n\r\nOK\r\n'
        text2 = '\r\n+CPIN: SIM PIN\r\n\r\nOK\r\n'
        match = extract.match(text2)
        self.failIf(match == None)
        self.assertEqual(match.group('resp'), 'SIM PIN')
        # [-] WAITING: DATA_RCV = '\r\n+CPIN: SIM PUK2\r\n\r\nOK\r\n'
        text3 = '\r\n+CPIN: SIM PUK2\r\n\r\nOK\r\n'
        match = extract.match(text3)
        self.failIf(match == None)
        self.assertEqual(match.group('resp'), 'SIM PUK2')
    
    def test_find_contacts_regexp(self):
        # [-] SENDING ATCMD 'AT+CPBF="0050"\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CPBF: 1,"+23434432",145,"0050006100620065006C0073"\r\n+CPBF: 53,"342239262",129,"005000410043004F0020004D0056002F004D"\r\n+CPBF: 36,"34233231481",129,"005000410043004F002F004D"\r\n+CPBF: 92,"43223963522",129,"0050004100500041002000500041005200540020004D0056002F004D"\r\n+CPBF: 93,"543453302",129,"005000410050004100200054005200420020004D0076002F004D"\r\n+CPBF: 103,"666",129,"005000610073006300750061006C002000560061006C002F004D"\r\n+CPBF: 109,"4534534532070",129,"00500045004F0050004C0045002F004D"\r\n+CPBF: 115,"623434212",129,"005000720069006D006F0020004E00630068006F002F004D"\r\n\r\nOK\r\n'
        extract = cmd_dict['find_contacts']['extract']
        text = '\r\n+CPBF: 1,"+23434432",145,"0050006100620065006C0073"\r\n+CPBF: 53,"342239262",129,"005000410043004F0020004D0056002F004D"\r\n+CPBF: 36,"34233231481",129,"005000410043004F002F004D"\r\n+CPBF: 92,"43223963522",129,"0050004100500041002000500041005200540020004D0056002F004D"\r\n+CPBF: 93,"543453302",129,"005000410050004100200054005200420020004D0076002F004D"\r\n+CPBF: 103,"666",129,"005000610073006300750061006C002000560061006C002F004D"\r\n+CPBF: 109,"4534534532070",129,"00500045004F0050004C0045002F004D"\r\n+CPBF: 115,"623434212",129,"005000720069006D006F0020004E00630068006F002F004D"\r\n\r\nOK\r\n'
        matches = list(re.finditer(extract, text))
        self.assertEqual(len(matches), 8)
        
        self.assertEqual(matches[0].group('name'), '0050006100620065006C0073')
        self.assertEqual(matches[0].group('number'), '+23434432')
        
        self.assertEqual(matches[7].group('name'), '005000720069006D006F0020004E00630068006F002F004D')
        self.assertEqual(matches[7].group('number'), '623434212')
    
    def test_get_available_charset(self):
        extract = cmd_dict['get_available_charset']['extract']
        text = '\r\n+CSCS: ("IRA","GSM","UCS2")\r\n'
        matches = list(re.finditer(extract, text))
        self.assertEqual(len(matches), 3)
        self.assertEqual(matches[0].group('lang'), "IRA")
        self.assertEqual(matches[2].group('lang'), "UCS2")
        
        text2 = '\r\n+CSCS: ("IRA")\r\n'
        matches = list(re.finditer(extract, text2))
        self.assertEqual(len(matches), 1)
        self.assertEqual(matches[0].group('lang'), "IRA")
        
        text3 = '\r\n+CSCS: ("IRA","8859-A","UCS2","PCDN")\r\n'
        matches = list(re.finditer(extract, text3))
        self.assertEqual(len(matches), 4)
        self.assertEqual(matches[0].group('lang'), "IRA")
        self.assertEqual(matches[1].group('lang'), "8859-A")
        
    def test_get_charset_regexp(self):
        # [-] SENDING ATCMD 'AT+CSCS?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CSCS: "UCS2"\r\n\r\nOK\r\n'
        extract = cmd_dict['get_charset']['extract']
        text = '\r\n+CSCS: "UCS2"\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('lang'), 'UCS2')
    
    def test_get_card_model_regexp(self):
        # [-] SENDING ATCMD 'AT+CGMM\r\n'
        # [-] WAITING: DATA_RCV = '\r\nE220\r\n\r\nOK\r\n'
        extract = cmd_dict['get_card_model']['extract']
        text = '\r\nE220\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('model'), 'E220')
    
    def test_get_card_version_regexp(self):
        # [-] SENDING ATCMD 'AT+GMR\r\n'
        # [-] WAITING: DATA_RCV = '\r\n11.110.01.03.00\r\n\r\nOK\r\n'
        extract = cmd_dict['get_card_version']['extract']
        text = '\r\n11.110.01.03.00\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('version'), '11.110.01.03.00')
    
    def test_get_contacts_regexp(self):
        # [-] SENDING ATCMD 'AT+CPBR=1,250\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CPBR: 1,"+23434432",145,"0050006100620065006C0073"\r\n\r\nOK\r\n'
        extract = cmd_dict['get_contacts']['extract']
        text = '\r\n+CPBR: 1,"+23434432",145,"0050006100620065006C0073"\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('number'), '+23434432')
        self.assertEqual(match.group('name'), '0050006100620065006C0073')
    
    def test_get_imei_regexp(self):
        # [-] SENDING ATCMD 'AT+CGSN\r\n'
        # [-] WAITING: DATA_RCV = '\r\n351834012602323\r\n\r\nOK\r\n
        extract = cmd_dict['get_imei']['extract']
        text = '\r\n351834012602323\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('imei'), '351834012602323')
    
    def test_get_imsi_regexp(self):
        # [-] SENDING ATCMD 'AT+CIMI\r\n'
        # [-] WAITING: DATA_RCV = '\r\n214012001727332\r\n\r\nOK\r\n'
        extract = cmd_dict['get_imsi']['extract']
        text = '\r\n214012001727132\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('imsi'), '214012001727132')
    
    def test_get_netreg_status_regexp(self):
        extract = cmd_dict['get_netreg_status']['extract']
        text = '\r\n+CREG: 0,1\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('mode'), '0')
        self.assertEqual(match.group('status'), '1')
    
    def test_get_network_info_regexp(self):
        # [-] SENDING ATCMD 'AT+COPS?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+COPS: 0,2,"21401",0\r\n\r\nOK\r\n'
        extract = cmd_dict['get_network_info']['extract']
        text = '\r\n+COPS: 0,2,"21401",0\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('netname'), '21401')
        self.assertEqual(match.group('status'), '0')
        
        text2 = '\r\n+COPS: 0,0,"vodafone ES",0\r\n\r\nOK\r\n'
        match2 = extract.match(text2)
        self.failIf(match2 == None)
        self.assertEqual(match2.group('netname'), 'vodafone ES')
        self.assertEqual(match2.group('status'), '0')

    def test_get_network_info_regexp_failure(self):
        # [-] SENDING ATCMD 'AT+COPS?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+COPS: 0\r\n\r\nOK\r\n'
        extract = cmd_dict['get_network_info']['extract']
        text = '\r\n+COPS: 0\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.failIf(int(match.group('error')), 0)
    
    def test_get_network_info_regexp_ucs2(self):
        extract = cmd_dict['get_network_info']['extract']
        text = '\r\n+COPS: 0,0,"0076006f006400610066006f006e0065002000450053",2\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('netname'), '0076006f006400610066006f006e0065002000450053')
        self.assertEqual(match.group('status'), '2')
    
    def test_get_network_names_ucs2_regexp(self):
        extract = cmd_dict['get_network_names']['extract']
        text = '\r\n+COPS: (1,"0076006f006400610066006f006e0065002000450053","0076006f00640061002000450053","21401",0),(2,"0076006f0064006100660)\r\n'
        matches = list(re.finditer(extract, text))
        self.failIf(matches == None)
        self.assertEqual(matches[0].group('lname'), '0076006f006400610066006f006e0065002000450053')
        self.assertEqual(matches[0].group('sname'), '0076006f00640061002000450053')
        self.assertEqual(matches[0].group('netid'), '21401')
    
    def test_get_network_names_ovation_regexp(self):
        """
        Novatel ovation's output wasnt matched by previous regexp
        """
        extract = cmd_dict['get_network_names']['extract']
        text = '\r\n+COPS: (1,"vodafone ES","voda ES","21401",0)\r\n+COPS: (2,"vodafone ES","voda ES","21401",2)\r\n+COPS: (1,"Orange","Orange","21403",2)\r\n+COPS: (1,"Yoigo","YOIGO","21404",2)\r\n+COPS: (1,"Orange","Orange","21403",0)\r\n+COPS: (1,"movistar","movistar","21407",0)\r\n+COPS: (1,"movistar","movistar","21407",2)\r\n\r\nOK\r\n'
        matches = list(re.finditer(extract, text))
        self.failIf(matches == None)
        self.assertEqual(matches[0].group('lname'), 'vodafone ES')
        self.assertEqual(matches[0].group('sname'), 'voda ES')
        self.assertEqual(matches[0].group('netid'), '21401')
    
    def test_get_phonebook_size(self):
        # [-] SENDING ATCMD 'AT+CPBR=?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CPBR: (1-250),40,16\r\n\r\nOK\r\n'
        extract = cmd_dict['get_phonebook_size']['extract']
        text = '\r\n+CPBR: (1-250),40,16\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('size'), '250')
    
    def test_get_pin_status(self):
        # [-] SENDING ATCMD 'AT+CLCK="SC",2\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CLCK: 1\r\n\r\nOK\r\n'
        extract = cmd_dict['get_pin_status']['extract']
        text = '\r\n+CLCK: 1\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('status'), '1')
    
    def test_get_signal_level_regexp(self):
        # [-] SENDING ATCMD 'AT+CSQ?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CSQ: 25,99\r\n\r\nOK\r\n'
        extract = cmd_dict['get_signal_level']['extract']
        text = '\r\n+CSQ: 25,99\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('rssi'), '25')
        self.assertEqual(match.group('ber'), '99')
    
    def test_get_smsc_regexp(self):
        # [-] SENDING ATCMD 'AT+CSCA?\r\n'
        # [-] WAITING: DATA_RCV = '\r\n+CSCA: "002B00330034003600300037003000300033003100310030",145\r\n\r\nOK\r\n'
        extract = cmd_dict['get_smsc']['extract']
        text = '\r\n+CSCA: "002B00330034003600300037003000300033003100310030",145\r\n\r\nOK\r\n'
        match = extract.match(text)
        self.failIf(match == None)
        self.assertEqual(match.group('smsc'), '002B00330034003600300037003000300033003100310030')
