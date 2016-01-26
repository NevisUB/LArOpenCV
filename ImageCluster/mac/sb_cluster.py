import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk

from ROOT import larcv

# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_io_mode(fmwk.storage_manager.kREAD)
 
myunit = fmwk.LArImageHit()
myunit.set_producer("gaushit")
myunit.set_config  ("ImageCluster/mac/SBCluster.fcl")

sb_algo = larcv.SmoothBinaryCluster()

plane_mgr0 = myunit.algo_manager(0);
plane_mgr0.AddAlg(sb_algo)

plane_mgr1 = myunit.algo_manager(1);
plane_mgr1.AddAlg(sb_algo)

plane_mgr2 = myunit.algo_manager(2);
plane_mgr2.AddAlg(sb_algo)


my_proc.add_process(myunit)

my_proc.process_event()

sys.exit(0)
