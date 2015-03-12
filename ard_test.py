#!/usr/bin/env python
"""
ard_test.py - Test the arduino without acquiring data.
"""

import trigger_qr_run as qr
import time

trig = qr.run_control()
time.sleep(3)
trig.trigger_run(100)
