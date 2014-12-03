#!/usr/bin/env python
# encoding: utf-8
"""
trigger_qr_run.py - interface with an arduino powering a quick release run

Created by Dave Williams on 2014-12-02
"""

import sys, os
import serial
from serial.tools import list_ports


class run_control(object):
    """Connect to the arduino and control QR runs as needed"""
    def __init__(self, port=None):
        """Takes the port to connect to as an argument, else connects to last
        port given by run_control.list_ports()
        """
        self._port = port if port is not None else self.list_ports()[-1]
        self._con = self._create_connection(self._port)
    
    @staticmethod
    def list_ports():
        """ List all serial ports"""
        ports = []
        if os.name == 'nt':
            #Windows
            for i in range(256):
                try:
                    s = serial.Serial(i)
                    s.close()
                    ports.append('COM' + str(i+1))
                except serial.SerialException:
                    pass
        else:
            #Mac/unix
            for port in list_ports.comports():
                ports.append(port[0]) 
        return ports
    
    @staticmethod
    def _create_connection(port):
        """Create a connection to a serial port"""
        ser = serial.Serial(port, baudrate = 115200) 
        return ser
    
    def trigger_run(self, step_to_exposure_delay):
        """Triggers a run, with a delay between the release and the exposure"""
        self._con.write(str(step_to_exposure_delay)+'\n')

