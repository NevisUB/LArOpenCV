import random
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.colors as mpc
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.patches as patches
import scipy
from scipy import interpolate
import pandas as pd
import itertools
from root_numpy import root2array, root2rec, tree2rec, array2root
import ROOT
from ROOT import geo2d
geo2d.load_pyutil
from ROOT.cv import Point_ as Vector
DTYPE='float'
pdraw = geo2d.PyDraw()
from ROOT import larocv

# configuration parameters for image generation
VERTEX_XMIN  = 50     # lower bound of x-range for random vtx
VERTEX_XMAX  = 50.01  # upper bound of x-range for random vtx
VERTEX_YMIN  = 50     # lower bound of y-range for random vtx
VERTEX_YMAX  = 50.01  # upper bound of y-range for random vtx
IMAGE_HEIGHT = 100    # image height
IMAGE_WIDTH  = 100    # image width
NOISE_LEVEL  = 0.0    # white noise level

NTRACKS          = [2,3]  # number of possible tracks to generate
TRACK_LENGTH_MAX = 100    # max track length
TRACK_LENGTH_MIN = 10     # min track length
# configuration parameter for algorithm testing
PI_THRESHOLD=0.5
RADIUS = 6

# Algorithm instance
alg = larocv.VertexParticleCluster()
alg._pi_threshold = PI_THRESHOLD
alg.set_verbosity(0)
larocv.logger.get_shared().set(0)
def gen_image():
    # Image size
    width,height=(IMAGE_WIDTH, IMAGE_HEIGHT)
    # Noise pixel fraction
    noise=NOISE_LEVEL
    # Vertex region
    vtx_xmin,vtx_xmax = (VERTEX_XMIN,VERTEX_XMAX)
    vtx_ymin,vtx_ymax = (VERTEX_YMIN,VERTEX_YMAX)
    # Track counts
    ntracks=NTRACKS
    # Track lenth
    tracklen_min,tracklen_max=(15,100)
    
    # Make background image
    data = np.zeros(width*height,DTYPE)
    data[int(len(data)*(1.-noise)):] = 1
    random.shuffle(data)
    data=data.reshape(height,width)
    
    #fig,ax = plt.subplots(figsize=(12,8))
    #im = plt.imshow(data, cmap='jet')
    #plt.colorbar(im, orientation='horizontal')
    #plt.show()
    
    # Generate vertex
    vtx   = Vector(DTYPE)()
    vtx.x = random.random() * (vtx_xmax - vtx_xmin) + vtx_xmin
    vtx.y = random.random() * (vtx_ymax - vtx_ymin) + vtx_ymin
    
    # Generate tracks
    random.shuffle(ntracks)
    ntrack = ntracks[0]
    
    # Generate angle
    segments=[]
    for idx in xrange(ntrack):
        pt1 = vtx
        pt2 = Vector(DTYPE)()
        angle = random.random() * 360
        
        line = geo2d.Line(DTYPE)(vtx,angle)
        
        pt2.x,pt2.y = (pt1.x,pt1.y)
        length = float(random.random() * (tracklen_max - tracklen_min) + tracklen_min)
        ctr = 0
        while length > ctr:
            pt2.x += line.dir.x
            pt2.y += line.dir.y
            if pt2.x < 0 or pt2.x > width or pt2.y < 0 or pt2.y > height:
                break
            x = int(pt2.x)
            y = int(pt2.y)
            data[y][x] += 1
            ctr+=1
            
        segments.append(geo2d.LineSegment(DTYPE)(pt1,pt2))
            
    fig,ax = plt.subplots(figsize=(12,8),facecolor='w')
    im = plt.imshow(data, cmap='jet')
    plt.colorbar(im, orientation='horizontal')
    #plt.show()
    return (vtx,segments,data)
        
vtx,lines,data = gen_image()
# figure out xs point
circle = geo2d.Circle(DTYPE)()
circle.center = vtx
circle.radius = RADIUS

# define a larocv::CircleVertex
vtx2d = larocv.data.CircleVertex()
vtx2d.center = vtx
vtx2d.radius = RADIUS
print
print len(lines),'crossings...'
for idx in xrange(len(lines)):
    line = lines[idx]
    pts = geo2d.IntersectionPoint(circle,line)
    for pt_idx in xrange(pts.size()):
        pt = pts[pt_idx]
        vtx2d.xs_v.push_back(larocv.data.PointPCA())
        vtx2d.xs_v.back().pt = pt
        print 'xs',idx,'@',vtx2d.xs_v.back().pt.x,',',vtx2d.xs_v.back().pt.y
print
data = pdraw.mat(data.astype(np.float32))
res=alg.CreateParticleCluster(data,vtx2d)
print res.size()
        
        
    
