#!/usr/bin/env python
# encoding: utf-8
"""
pilatus.py - connect to and control the pilatus100

NB: cam_server must be on and epics GUI must be off for this to work
If something looks wrong, debug by watching the output on the cam_server window

Created by Dave Williams on 2014-12-04
"""

import os
import time
import epics
from epics import pv


#TODO: Pass in basedir

BASE_DIR = '/nas_data/2016DanielQR/'

BASE_EPICS = "18ID-2:PILATUS1:"

def set_up_pilatus(fn, exp_time, base_dir=BASE_DIR):
    """Configure the pilatus for an acquisition"""
    # Validate the name
    if not fn.split('.')[-1].startswith('tif'):
            fn += '.tiff'
    # Set up pv connections
    expose_entry = pv.PV(BASE_EPICS+"AcquireTime")
    aquire_button = pv.PV(BASE_EPICS+"Acquire")
    filepath_entry = pv.PV(BASE_EPICS+"FilePath")
    filename_entry = pv.PV(BASE_EPICS+"FileName")
    template_entry = pv.PV(BASE_EPICS+"FileTemplate")
    trigger_entry = pv.PV(BASE_EPICS+"TriggerMode")
    # Set up messages to send
    exp_msg = (exp_time/1000000.0) # convert to sec
    fn_msg = fn
    # Send messages
    expose_entry.put(exp_msg)
    filepath_entry.put(base_dir)
    filename_entry.put(fn)
    aquire_button.put(1)
    template_entry.put("%s%s")
    trigger_entry.put(2)
    return base_dir+fn