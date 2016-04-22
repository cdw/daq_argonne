#!/usr/bin/env python
# encoding: utf-8
"""
run_run.py - run a run, no walking allowed

Created by Dave Williams on 2014-12-04
"""

import sys
import os
import time
import pickle
import json
import numpy as np
import daq_control
import trigger_qr_run 
import pilatus
import matplotlib.pyplot as plt

def main(argv=None):
    ## Manage input arguments
    if argv is None:
        argv = sys.argv
    print argv
    ## Argv should now consist of [species, fiber_number, 
    ##                             exposure_delay, fiber_offset,
    ##                             trial_number]
    species, fiber_number, exposure_delay, fiber_offset, trial_number = argv
    #Create input dict
    fiber_number = fiber_number
    exposure_delay = int(exposure_delay)
    fiber_offset = float(fiber_offset)
    trial_number = int(trial_number)
    input_dict = {'fiber_number':fiber_number,
                 'exposure_delay':exposure_delay,
                 'fiber_offset':fiber_offset,
                 'trial_number':trial_number}
    data = grab(input_dict)
    ## Plot data
    plot(data)
    return data

def grab(input_dict):
    """Grab a trial with parameters set by input_dict which must contain:
     - fiber_number: what fiber we are on
     - exposure_delay: how long to wait between length change and image in microsec
     - fiber_offset: what our lateral offset is relative to the motor arm
     - trial_number: image trial 
     - exposure_length: how long our exposure will be in microsec
    """
    # Coerce data into needed formats
    fiber_number = input_dict['fiber_number']
    exposure_delay = int(input_dict['exposure_delay'])
    fiber_offset = float(input_dict['fiber_offset'])
    trial_number = int(input_dict['trial_number'])
    exposure_length = int(input_dict['exposure_length'])
    ## Set up detector
    image_name = "T%04i_Delay%04i_Offset%02.3f_Fiber%i"%(trial_number, exposure_delay,
                                                         fiber_offset, fiber_number)
    print "Trig wait with %s"%image_name
    image_path = pilatus.set_up_pilatus(image_name, exposure_length)
    time.sleep(2) # allow camera to ready, possibly not needed
    ## Acquire data
    arduino = trigger_qr_run.run_control()
    time.sleep(2) # allow serial to connect
    daq = daq_control.daq_acquire()
    daq.start_task()
    arduino.trigger_run(exposure_delay)
    daq.read_data()
    data = daq.return_dict()
    ## Add in image name and input dict to data
    data['image_path'] = image_path
    data.update(input_dict)
    ## Write out data
    output_file = open('./trials/T%04i.pkl'%trial_number, 'wb')
    pickle.dump(data, output_file)
    output_file.close()
    json_file = open('./trials/T%04i.json'%trial_number, 'wb')
    json.dump(data, json_file)
    json_file.close()
    return data

def plot(data):
    """Plot the data from a run"""
    fig, axes = plt.subplots(nrows = 4, ncols = 2, figsize=(12,8.5))
    axes = axes.flatten()
    plt.subplots_adjust(left=.15, bottom=.06, right=.95, top=.97, 
                        wspace=.17, hspace=.30)
    curr = axes[0]
    curr.set_ylabel('length')
    curr.set_title('L (V)')
    curr.plot(data['length'])
    curr = axes[1]
    curr.set_ylabel('F (V)')
    curr.set_title('Force')
    curr.plot(data['force'])
    curr = axes[2]
    curr.set_ylabel('Stim (V)')
    curr.set_title('Stimulation')
    curr.plot(data['stimulation'])
    curr = axes[3]
    curr.set_ylabel('Beam (BPM diode)')
    curr.set_title('Beam')
    curr.plot(data['beam'])
    curr = axes[4]
    curr.set_ylabel('Exposure trigger (V)')
    curr.set_title('Pilatus')
    curr.plot(data['exposure'])
    curr = axes[5]
    curr.set_title('PSD1')
    curr.plot(data['psd1'])
    curr = axes[6]
    curr.set_title('PSD2')
    curr.plot(data['psd2'])
    curr = axes[7]
    curr.set_title('PSD diff over sum')
    diff_over_sum = lambda p1, p2: np.subtract(p1, p2)/np.add(p1,p2)
    curr.plot(diff_over_sum(data['psd1'], data['psd2']))
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    #sys.exit(main())
    main()

