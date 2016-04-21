# -*- coding: utf-8 -*-
"""
run_acquire

Collect data for a run.
Inputs:
 - stimulator signal
 - motor length
 - beam shutter
 - picture trigger
 - force transducer
 - laser follower
Possible outputs:
 - length control (digital)
 - beam control (digital)
 - image exposure control (digital)
"""

import numpy as np
import nidaqmx


class daq_acquire(object):
    """Configure and acquire data from a NI USB DAQ"""
    def __init__(self):
        """Set up the channels and such"""
        ## Input channel configuration
        self.nchannels = 7
        self.channelnames = ['length',      # muscle lever output
                             'force',       # force transducer output
                             'stimulation', # sync signal from TI's stim
                             'beam',        # bpm diode upstream of sample
                             'exposure',    # pilatus trigger signal
                             'psd1',        # output from top psd connection
                             'psd2']        # bottom psd connection
        ## Sample rate configuration
        self.samplerate = 10000
        self.nsamples = 10000 # if samplemode = 'continuous', 
                              # this determines buffer size
        self.samplemode = 'finite' # or 'continuous'
        self.TERMINALEND = 'diff' 
            # consider 'rse' (referenced single-ended),'nrse'
            # (non-referenced single ended), differential,
            # for configuration of analog input
        ## Initialize task
        self.configure_task()
        ## Dummy data for now
        self.data = None
    
    def configure_task(self):
        """Set up an analog input task"""
        n = str(self.nchannels - 1)
        self.itask = nidaqmx.AnalogInputTask()
        self.itask.create_voltage_channel(
                                r'Dev1/ai0:'+n, # create n AIs
                                min_val=-10.0,  # min at -10V
                                max_val=10.0,   # max at 10V
                                terminal=self.TERMINALEND)
        self.itask.configure_timing_sample_clock(rate=self.samplerate,
                                      sample_mode=self.samplemode,
                                      samples_per_channel=self.nsamples)
    
    def start_task(self):
        """Begin the collection and wait for it to be completed"""
        self.itask.start()
    
    def read_data(self):
        """Read the data off the DAQ's buffer"""
        self.itask.wait_until_done(10.0)
        raw = self.itask.read()
        deinterleaved = [[] for i in raw[0]]
        for sample in raw:
            for i, datum in enumerate(sample):
                deinterleaved[i].append(datum)
        self.data = deinterleaved
        print "done"
    
    def return_data(self):
        """Return the (possibly dummy) data"""
        return self.data
    
    def return_dict(self):
        """Return data as a dictionary with expected channel labels"""
        labels = self.channelnames
        datadict = dict(zip(labels, self.data)) 
        datadict['sample_rate'] = self.samplerate
        datadict['sample_number'] = self.nsamples
        datadict['terminal'] = self.TERMINALEND
        return datadict
