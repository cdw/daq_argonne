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
import daq_control
import trigger_qr_run 


def main(argv=None):
    ## Manage input arguments
    if argv is None:
        argv = sys.argv
    print argv
    ## Argv should now consist of [filepath,species, fiber_number, 
    ##                             exposure_delay, fiber_offset,
    ##                             trial_number]
    filepath, species, fiber_number, exposure_delay, fiber_offset, trial_number = argv
    fiber_number = int(fiber_number)
    exposure_delay = int(exposure_delay)
    fiber_offset = float(fiber_offset)
    trial_number = int(trial_number)
    time.sleep(2) # allow serial to connect
    ## Acquire data
    arduino = trigger_qr_run.run_control()
    daq = daq_control.daq_acquire()
    daq.start_task()
    arduino.trigger_run(exposure_delay)
    daq.read_data()
    data = daq.return_dict()
    ## Add in fiber offset and exposure delay to data dict
    data['species'] = species
    data['fiber_number'] = fiber_number
    data['exposure_delay'] = exposure_delay
    data['fiber_offset'] = fiber_offset
    data['trial'] = trial_number
    ## Write out data
    output_file = open('./trials/T%04i.pkl'%trial_number, 'w')
    pickle.dump(data, output_file)
    output_file.close()
    ## Plot data
    import matplotlib.pyplot as plt
    fig, axes = plt.subplots(nrows = 5, ncols = 1, figsize=(6,7))
    plt.subplots_adjust(left=.15, bottom=.06, right=.95, top=.97, 
                        wspace=.17, hspace=.30)
    curr = axes[0]
    curr.set_ylabel('length')
    curr.plot(data['length'])
    curr = axes[1]
    curr.set_ylabel('force')
    curr.plot(data['force'])
    curr = axes[2]
    curr.set_ylabel('stimulation')
    curr.plot(data['stimulation'])
    curr = axes[3]
    curr.set_ylabel('beam')
    curr.plot(data['beam'])
    curr = axes[4]
    curr.set_ylabel('exposure')
    curr.plot(data['exposure'])
    plt.show()
    return data



if __name__ == '__main__':
	#sys.exit(main())
    main()

