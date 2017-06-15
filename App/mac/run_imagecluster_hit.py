import sys

if len(sys.argv) < 4:
    msg  = '\n'
    msg += "Usage : python %s CONFIG.fcl processname $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk

my_proc = fmwk.ana_processor()

cfg  = sys.argv[1]
name = sys.argv[2]
if not cfg.endswith('.fcl'):
    print 'Config file needs to end with \'.fcl\' extension (sorry bad joke)'
    sys.exit(1)
	
	
#add additional -1 so I can send in the event number as the last sysarg
for x in xrange(len(sys.argv) - 2 - 1): 
    my_proc.add_input_file(sys.argv[x+3])

my_proc.set_io_mode(fmwk.storage_manager.kBOTH)
my_proc.set_output_file("%s_output.root" % name); 

myunit = fmwk.LArImageHit()
myunit.set_config(cfg)
my_proc.add_process(myunit)

my_proc.set_data_to_write(fmwk.data.kCluster,    "ImageClusterHit")
my_proc.set_data_to_write(fmwk.data.kPFParticle, "ImageClusterHit")
my_proc.set_data_to_write(fmwk.data.kAssociation,"ImageClusterHit")

my_proc.run()
             

sys.exit(0)
