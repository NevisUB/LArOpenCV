import sys
from ROOT import gSystem
gSystem.Load("libOpenCV_Play")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing Play..."

sys.exit(0)

