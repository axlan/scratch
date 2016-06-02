import peregrine.samples
import sys
import peregrine.analysis.acquisition
import peregrine.log; peregrine.log.docs_logging_config()
import numpy as np
samps = peregrine.samples.load_samples(sys.argv[1])
print len(samps)
print np.unique(samps)
acq = peregrine.acquisition.Acquisition(samps, 16.368e6, 4.092e6, 16368)
res = acq.acquisition(show_progress=False); res

