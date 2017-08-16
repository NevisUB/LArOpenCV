import ROOT
from ROOT import larocv

print larocv

#
# Instantiate the algorithm manager
#
alg_mgr = larocv.ImageClusterManager()

#
# Load the configuration for algorithm manager
#
config = larocv.CreatePSetFromFile("../image_cluster_manager.cfg","ImageClusterManager")

print "Loaded config"
print config.dump()
print

alg_mgr.Configure(config)


#
# Give images to the algorithm
#

import cv2
import numpy as np

#
# Read in the ADC images
#
adc0 = cv2.imread("../img/adc.001.jpeg",cv2.IMREAD_GRAYSCALE)
adc1 = cv2.imread("../img/adc.002.jpeg",cv2.IMREAD_GRAYSCALE)
adc2 = cv2.imread("../img/adc.003.jpeg",cv2.IMREAD_GRAYSCALE)

adc0 = 255 - adc0
adc1 = 255 - adc1
adc2 = 255 - adc2

adc_v = [adc0,adc1,adc2]

#
# Read in the Track images
#

trk0 = cv2.imread("../img/track.001.jpeg",cv2.IMREAD_GRAYSCALE)
trk1 = cv2.imread("../img/track.002.jpeg",cv2.IMREAD_GRAYSCALE)
trk2 = cv2.imread("../img/track.003.jpeg",cv2.IMREAD_GRAYSCALE)

trk0 = 255 - trk0
trk1 = 255 - trk1
trk2 = 255 - trk2

trk_v = [trk0,trk1,trk2]

#
# Read in the shower images
#
shr0 = cv2.imread("../img/shower.001.jpeg",cv2.IMREAD_GRAYSCALE)
shr1 = cv2.imread("../img/shower.002.jpeg",cv2.IMREAD_GRAYSCALE)
shr2 = cv2.imread("../img/shower.003.jpeg",cv2.IMREAD_GRAYSCALE)

shr0 = 255 - shr0
shr1 = 255 - shr1
shr2 = 255 - shr2

shr_v = [shr0,shr1,shr2]


#
# Use Geo2D module to convert np.array to cv::Mat
# to feed to the algorithm manager
#

from ROOT import geo2d
pydraw = geo2d.PyDraw()

# Insert ADC images
print "Inserting shower images..."
for plane,img in enumerate(adc_v):
    mat  = pydraw.mat(img)
    meta = larocv.ImageMeta(mat.rows,mat.cols,
                            mat.rows,mat.cols,
                            0,0,
                            plane)

    alg_mgr.Add(mat,meta,0) # 0 == larocv::ImageSetID_t::kImageSetWire
print "... done"
print

# Insert Track images
print "Inserting track images..."
for plane,img in enumerate(trk_v):
    mat  = pydraw.mat(img)
    meta = larocv.ImageMeta(mat.rows,mat.cols,
                            mat.rows,mat.cols,
                            0,0,
                            plane)

    alg_mgr.Add(mat,meta,1) # 1 == larocv::ImageSetID_t::kImageSetTrack
print "... done"
print

# Insert Shower images
print "Inserting shower images..."
for plane,img in enumerate(trk_v):
    mat  = pydraw.mat(img)
    meta = larocv.ImageMeta(mat.rows,mat.cols,
                            mat.rows,mat.cols,
                            0,0,
                            plane)

    alg_mgr.Add(mat,meta,2) # 2 == larocv::ImageSetID_t::kImageSetShower
print "... done"
print

#
# Call reconstruct 
#
print "Processing!"
alg_mgr.Process()
print "...done"
print

#
# Get the algorithm data
#
algo_name = "adc_superclustermaker"
algo_id = alg_mgr.GetClusterAlgID(algo_name)
print "Got",algo_name,"@ id=",algo_id
print

data_man = alg_mgr.DataManager()

data_v = [None,None,None]
for plane in xrange(3):
    data_v[plane] = data_man.Data(algo_id,plane)

print "Got AlgoData..."
print data_v
print


import matplotlib
matplotlib.use('agg')

import matplotlib.pyplot as plt

plt.imshow(adc_v[0],cmap='Greys',interpolation='none')

for particle in data_v[0].as_vector():
    x = []
    y = []
    for pt in particle._ctor:
        x.append(pt.x)
        y.append(pt.y)

    plt.plot(x,y,'-')
    
plt.savefig("adc_contour_0.png",type="png")
