#!/usr/bin/env python
# encoding: utf-8
"""
pilatus.py - connect to and control the pilatus100

NB: cam_server must be on and epics GUI must be off for this to work
If something looks wrong, debug by watching the output on the cam_server window

Created by Dave Williams on 2014-12-04
"""

import socket

class pilatus(object):
    """Connect to and configure the pilatus100 detector"""
    def __init__(self):
        """Set up the socket connection"""
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        pilatus_address = ('164.54.204.70', 41234)
        self.conn.connect(pilatus_address)
        self._exp_time = 100 # allows auto-period updates
        self.img_num = 1
        self.exp_time = 100
        self.base_dir = '/nas_data/2016DanielQR/'
    
    @property
    def img_num(self):
        """Number of images in next exposure set"""
        return self._img_num
    
    @img_num.setter
    def img_num(self, new_img_num):
        self._img_num = new_img_num
        self.conn.sendall("NImages = %i"%new_img_num + ' '*20)
        self.update_period()
    
    @property
    def exp_time(self):
        """Exposure time in microseconds"""
        return self._exp_time

    @exp_time.setter
    def exp_time(self, new_exp_time):
        self._exp_time = new_exp_time
        # Note conversion from microsec to seconds for writing
        self.conn.sendall("ExpTime = %0.7f"%(new_exp_time/1000000.0) + ' '*20)
        self.update_period()

    def update_period(self):
        """Exposure period should be Nimages * ExpPeriod + Read Time"""
        exp_per = self.img_num*self.exp_time/1000000.0 + 0.010
        self.conn.sendall("ExpPeriod = %0.7f"%exp_per + ' '*20)

    def trig_wait(self, out_name):
        """Wait for an external trigger, writing output to out_name"""
        if not out_name.split('.')[-1].startswith('tif'):
            out_name += '.tif'
        self.conn.sendall("ExtTrigger %s"%(self.base_dir+out_name) + ' '*20)

    def __del__(self):
        """Kill the connection"""
        self.conn.shutdown(1)
        self.conn.close()
    
