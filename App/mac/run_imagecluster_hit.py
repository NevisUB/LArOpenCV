import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
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


for x in xrange(len(sys.argv)-3):
    my_proc.add_input_file(sys.argv[x+3])

my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

my_proc.set_output_file("%s_output.root" % name); 

myunit = fmwk.LArImageHit()
# myunit = fmwk.LArImageWire()
myunit.set_config(cfg)
my_proc.add_process(myunit)
my_proc.run()

sys.exit(0)
