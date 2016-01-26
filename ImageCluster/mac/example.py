import sys
from ROOT import gSystem
gSystem.Load("libOpenCV_ImageCluster")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing ImageCluster..."

sys.exit(0)

