import sys
from ROOT import gSystem
gSystem.Load("libLArOpenCV_YoloAPI")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing YoloAPI..."

sys.exit(0)

