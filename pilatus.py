#!/usr/bin/env python
# encoding: utf-8
"""
pilatus.py - connect to and control the pilatus100

NB: cam_server must be on and epics GUI must be off for this to work
If something looks wrong, debug by watching the output on the cam_server window

Created by Dave Williams on 2014-12-04
"""

import socket


BASE_DIR = '/nas_data/2016DanielQR/'
READ_TIME = 0.1 #minimal readout time, in seconds
IMG_NUM = 1
ADDRESS = ('164.54.204.70', 41234)


def send_message(sock, message):
    """Send a message to the pilatus over a socket, then close it"""
    mess_len = len(message)
    totalsent = 0
    while totalsent<mess_len:
        sent = sock.send(message[totalsent:])
        if sent == 0:
            raise RuntimeError("socket connection broken")
        totalsent = totalsent + sent

def set_up_pilatus(fn, exp_time):
    """Configure the pilatus for an acquisition"""
    # Validate the name
    if not fn.split('.')[-1].startswith('tif'):
            fn += '.tiff'
    # Set up the socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(ADDRESS)
    # Send the messages
    img_msg = "NImages = %i\n"%IMG_NUM
    exp_msg = "ExpTime = %0.7f\n"%(exp_time/1000000.0) # convert to sec
    per_msg = "ExpPeriod = %0.7f\n"%(IMG_NUM*exp_time/1000000.0 + 0.100)
    fn_msg = "ExtTrigger %s\n"%(BASE_DIR+fn)
    send_message(sock, img_msg)
    send_message(sock, exp_msg)
    send_message(sock, per_msg)
    send_message(sock, fn_msg)
    sock.shutdown(1)
    sock.close()
    return BASE_DIR+fn
