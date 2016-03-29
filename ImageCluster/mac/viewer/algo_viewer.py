from viewer_methods import *
import matplotlib.patches as mpatches
from matplotlib.collections import PatchCollection
import matplotlib

import numpy as np

class AlgoViewer :
    def __init__ (self,algo,plt):
        self.algo  = algo
        self.name = algo.Name()
        
        self.plt  = plt
        
    def draw(self,ax,cluster,cindex,meta):
        def pixel_y(y):
            y = int(y)
            if y < 0:
                y = 0
            return meta.pixel_y(y)


        def pixel_x(x):
            x = int(x)
            if x < 0:
                x = 0
            return meta.pixel_x(x)
        
    
        if self.name in ["sbc"] :
            ax.plot(meta.roivtx().y,
                    meta.roivtx().x,'*',markersize=10,color='orange')
            return
        
        if self.name in ["Trackshower"] :
            
            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,meta)
            ax.plot(px,py,'o',markersize=1,color='black')
            
            bbox = cluster._minAreaRect
            if bbox.size() != 0 :
                ax.plot([pixel_y(bbox[0].x),
                         pixel_y(bbox[1].x),
                         pixel_y(bbox[2].x),
                         pixel_y(bbox[3].x),
                         pixel_y(bbox[0].x)],
                        [pixel_x(bbox[0].y),
                         pixel_x(bbox[1].y),
                         pixel_x(bbox[2].y),
                         pixel_x(bbox[3].y),
                         pixel_x(bbox[0].y)],color='orange',lw=2)
                
                ax.plot(pixel_y(cluster._startPt.x),
                        pixel_x(cluster._startPt.y),'o',color='pink',markersize=5)
            
            ax.plot(pixel_y(cluster._endPt.x),
                    pixel_x(cluster._endPt.y),'o',color='green',markersize=5)

        if self.name in ["gs"] :
            
            insidehits = cluster._insideHits

            px, py     = get_xy_w_offset(insidehits,meta)
            ax.plot(px,py,'o',markersize=1,color='black')

            #Ignore this cluster if less than 25 hits, or its polygon has no found vertices
	    if cluster._insideHits < 25 or cluster._endPt.x < 0: 
	        return

	    if not cluster.PolyObject.Size(): 
	     return

            #Plot polygons
	    pts_x = []
	    pts_y = []
	    for pt in xrange(cluster.PolyObject.Size()):
	      pts_x.append( pixel_y(cluster.PolyObject.Point(pt).first))
	      pts_y.append( pixel_x(cluster.PolyObject.Point(pt).second))
	     # ax.plot(cluster.PolyObject.Point(pt).first + xs, cluster.PolyObject.Point(pt).second + ys,'o',color='red',markersize=10)

	    pts_x.append( pixel_y(cluster.PolyObject.Point(0).first))
	    pts_y.append( pixel_x(cluster.PolyObject.Point(0).second))

	    ax.plot(pts_x,pts_y,color='red',lw=2)
            
	    #Plot start + end
            ax.plot(pixel_y(cluster._startPt.x),
                    pixel_x(cluster._startPt.y),'o',color='green',markersize=10)

            ax.plot(pixel_y(cluster._endPt.x),
                    pixel_x(cluster._endPt.y),'o',color='pink',markersize=5)
            
            
        if self.name in ["pcas"] :

            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,meta)
            ax.plot(px,py,'o',markersize=1,color='black')

            #This sometimes happpens for some reason
            if cindex >= self.algo._pcapaths.size():
                cindex = self.algo._pcapaths.size() - 1
                
            path   = self.algo._pcapaths[cindex];

            nboxes = path.chosen_boxes_.size()

            print "WARNING CHECK rectangle == BLOCK !!!"
            
            for b in xrange(nboxes):
                box       = path.chosen_boxes_[b]
                rectangle = self.plt.Rectangle((pixel_y(box.x), pixel_x(box.y)),
                                               pixel_height()*pixelbox.width, pixel_width()*box.height,
                                               fc='white',ec='black',alpha=1,lw=3)
                ax.add_patch(rectangle)
                

            ax.plot(pixel_y(cluster._startPt.x),
                    pixel_x(cluster._startPt.y),'o',color='pink',markersize=10)

            ax.plot(pixel_y(cluster._endPt.x),
                    pixel_x(cluster._endPt.y),'o',color='green',markersize=10)

            evf = cluster._eigenVecFirst;
            evc = cluster._centerPt;
            bbb = cluster._boundingBox;
                
            a = get_bounded_line(evf,evc,bbb)
            ax.plot(pixel_y(a[0]),
                    pixel_x(a[1]),'-',color='red',lw=2)
            
        if self.name in ["pizerofilter"] :

            ax.plot (pixel_y(cluster._vertex_2D.x),
                     pixel_x(cluster._vertex_2D.y),'o',markersize=10,color='black')
            
            ax.plot([pixel_y(cluster._startPt.x),pixel_y(cluster._vertex_2D.x)],
                    [pixel_x(cluster._startPt.y),pixel_x(cluster._vertex_2D.y)],'-o',color='pink',lw=3)

        if self.name in ["pizerofilterv"] :
            
            ##################TEMP COPY################
            insidehits = cluster._insideHits

            px, py     = get_xy_w_offset(insidehits,meta)
            ax.plot(px,py,'o',markersize=1,color='black')

            #Ignore this cluster if less than 25 hits, or its polygon has no found vertices
	    if cluster._insideHits < 25 or cluster._endPt.x < 0: 
	        return
            
	    if not cluster.PolyObject.Size(): 
	        return

            #Plot polygons
	    pts_x = []
	    pts_y = []
	    for pt in xrange(cluster.PolyObject.Size()):
	      pts_x.append(pixel_y(cluster.PolyObject.Point(pt).first))
	      pts_y.append(pixel_x(cluster.PolyObject.Point(pt).second))	     # ax.plot(cluster.PolyObject.Point(pt).first + xs, cluster.PolyObject.Point(pt).second + ys,'o',color='red',markersize=10)

	    pts_x.append( pixel_y(cluster.PolyObject.Point(0).first))
	    pts_y.append( pixel_x(cluster.PolyObject.Point(0).second))

	    ax.plot(pts_x,pts_y,color='red',lw=2)
            
	    #Plot start + end
            ax.plot(pixel_y(cluster._startPt.x),
                    pixel_x(cluster._startPt.y),'o',color='green',markersize=10)

            ax.plot(pixel_y(cluster._endPt.x),
                    pixel_x(cluster._endPt.y),'o',color='pink',markersize=5)
            ###################TEMP COPY###################
            
            _verts = self.algo._verts_v[ cluster.PlaneID() ]
            
            for v in xrange(_verts.size()):
                vert = _verts[ v ];
                ax.plot(pixel_y(vert._vtx.x),pixel_x(vert._vtx.y),'*',markersize=10)
                ax.plot([pixel_y(vert._vtx.x),pixel_y(vert.first._startPt.x)],
                        [pixel_x(vert._vtx.y),pixel_x(vert.first._startPt.y)],'-',lw=2,color='gray')

                ax.plot([pixel_y(vert._vtx.x),pixel_y(vert.second._startPt.x)],
                        [pixel_x(vert._vtx.y),pixel_x(vert.second._startPt.y)],'-',lw=2,color='gray')

        if self.name in ["brs","csd","mnc"]:
            bbox = cluster._minAreaRect
            
            if bbox.size() != 0 :
                ax.plot([pixel_y(bbox[0].x),
                         pixel_y(bbox[1].x),
                         pixel_y(bbox[2].x),
                         pixel_y(bbox[3].x),
                         pixel_y(bbox[0].x)],
                        [pixel_x(bbox[0].y),
                         pixel_x(bbox[1].y),
                         pixel_x(bbox[2].y),
                         pixel_x(bbox[3].y),
                         pixel_x(bbox[0].y)],
                        color='orange',lw=2)

            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,meta)
            ax.plot(px,py,'o',markersize=1,color='black')

            ax.plot(pixel_y(cluster._startPt.x),
                    pixel_x(cluster._startPt.y),'o',color='pink'  ,markersize=10)
            
            ax.plot(pixel_y(cluster._endPt.x),
                    pixel_x(cluster._endPt.y),'o',color='green',markersize=10)
            
            nboxes = cluster._verts.size();

            r = 0
            for v in xrange(nboxes):
               box = cluster._verts[v]
               xx = [pixel_y(box[0].x),
                     pixel_y(box[1].x),
                     pixel_y(box[2].x),
                     pixel_y(box[3].x),
                     pixel_y(box[0].x)]
               yy = [pixel_x(box[0].y),
                     pixel_x(box[1].y),
                     pixel_x(box[2].y),
                     pixel_x(box[3].y),
                     pixel_x(box[0].y)]

               a = np.array(zip(xx,yy))
               polygon = mpatches.Polygon(a,True,fc=str(cluster._something[r]))

               ax.add_patch(polygon)
               r+=1
