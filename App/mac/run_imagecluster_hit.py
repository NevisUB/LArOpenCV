import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

import ROOT
from larlite import larlite as fmwk
from ROOT import larcv
# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
myunit = fmwk.LArImageHit()
myunit.algo_manager(0).AddAlg(larcv.ToyImageCluster())
myunit.algo_manager(1).AddAlg(larcv.ToyImageCluster())
myunit.algo_manager(2).AddAlg(larcv.ToyImageCluster())
my_proc.add_process(myunit)

print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
my_proc.run(0,10);

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
