import sys
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk

my_proc = fmwk.ana_processor()

for x in xrange(len(sys.argv)-1):
    
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_io_mode(fmwk.storage_manager.kREAD)

my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root");

my_unit = fmwk.HitImageMaker()

my_proc.add_process(my_unit)

print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
while my_proc.process_event():

    img = my_unit.GetImage(2)
    imgplot = plt.imshow(img)
    plt.show()

    canny = my_unit.GetCanny(2)
    cannyplot = plt.imshow(canny)
    plt.show()
    
    num_contours = my_unit.NumContours(2)
    print num_contours,'contours found'
    contourplot = plt.imshow(canny)
    for x in xrange(num_contours):
        x_array,y_array = my_unit.GetContour(2,x)
        plt.plot(x_array,y_array)

    plt.show()
    sys.stdin.readline()

    #break

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
