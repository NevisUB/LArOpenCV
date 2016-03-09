from viewer_methods import get_xy, get_xy_w_offset
import matplotlib.patches as mpatches
from matplotlib.collections import PatchCollection
import matplotlib

import numpy as np

class AlgoViewer :
    def __init__ (self,algo,plt):
        self.algo  = algo
        self.name = algo.Name()
        
        self.plt  = plt
        
    def draw(self,ax,cluster,cindex):
        xs = cluster.Origin().y
        ys = cluster.Origin().x
            
        if self.name in ["sbc"] :
            return

        if self.name in ["Trackshower"] :
            
            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,xs,ys)
            ax.plot(px,py,'o',markersize=1,color='black')
            
            bbox = cluster._minAreaRect
            if bbox.size() != 0 :
                ax.plot([bbox[0].x+xs,bbox[1].x+xs,bbox[2].x+xs,bbox[3].x+xs,bbox[0].x+xs],
                        [bbox[0].y+ys,bbox[1].y+ys,bbox[2].y+ys,bbox[3].y+ys,bbox[0].y+ys],color='orange',lw=2)

            ax.plot(cluster._startPt.x + xs,
                    cluster._startPt.y + ys,'o',color='pink',markersize=5)

            ax.plot(cluster._endPt.x + xs,
                    cluster._endPt.y + ys,'o',color='green',markersize=5)
            
            
        if self.name in ["pcas"] :

            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,xs,ys)
            ax.plot(px,py,'o',markersize=1,color='black')

            #This sometimes happpens for some reason
            if cindex >= self.algo._pcapaths.size():
                cindex = self.algo._pcapaths.size() - 1
                
            path   = self.algo._pcapaths[cindex];

            nboxes = path.chosen_boxes_.size()

            for b in xrange(nboxes):
                box       = path.chosen_boxes_[b]
                rectangle = self.plt.Rectangle((box.x + xs, box.y + ys),
                                               box.width, box.height,
                                               fc='white',ec='black',alpha=1,lw=3)
                ax.add_patch(rectangle)
                
            bbox = cluster._minAreaRect
            if bbox.size() != 0 :
                ax.plot([bbox[0].x+xs,bbox[1].x+xs,bbox[2].x+xs,bbox[3].x+xs,bbox[0].x+xs],
                        [bbox[0].y+ys,bbox[1].y+ys,bbox[2].y+ys,bbox[3].y+ys,bbox[0].y+ys],color='orange',lw=2)

            ax.plot(cluster._startPt.x + xs,
                    cluster._startPt.y + ys,'o',color='pink',markersize=5)

            ax.plot(cluster._endPt.x + xs,
                    cluster._endPt.y + ys,'o',color='green',markersize=5)
            
        if self.name in ["pizerofilter"] :

            ax.plot (xs+cluster._vertex_2D.x,
                     ys+cluster._vertex_2D.y,'o',markersize=10,color='black')
            
            ax.plot([cluster._startPt.x + xs,xs+cluster._vertex_2D.x],
                    [cluster._startPt.y + ys,ys+cluster._vertex_2D.y],'-o',color='pink',lw=3)
            
        if self.name in ["brs","csd","mnc"]:
            bbox = cluster._minAreaRect
            
            if bbox.size() != 0 :
                ax.plot([bbox[0].x+xs,bbox[1].x+xs,bbox[2].x+xs,bbox[3].x+xs,bbox[0].x+xs],
                        [bbox[0].y+ys,bbox[1].y+ys,bbox[2].y+ys,bbox[3].y+ys,bbox[0].y+ys],color='orange',lw=2)

            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,xs,ys)
            ax.plot(px,py,'o',markersize=1,color='black')

            ax.plot(cluster._startPt.x + xs,
                   cluster._startPt.y + ys,'o',color='pink'  ,markersize=20)

            ax.plot(cluster._endPt.x + xs,
                    cluster._endPt.y + ys,'o',color='green',markersize=20)
            
            
            nboxes = cluster._verts.size();
            #print "nboxes",nboxes
            r = 0
            for v in xrange(nboxes):
               box = cluster._verts[v]
               xx = [box[0].x+xs,box[1].x+xs,box[2].x+xs,box[3].x+xs,box[0].x+xs]
               yy = [box[0].y+ys,box[1].y+ys,box[2].y+ys,box[3].y+ys,box[0].y+ys]
               #ax.plot(xx,yy,lw=2)
               a = np.array(zip(xx,yy))
               polygon = mpatches.Polygon(a,True,fc=str(cluster._something[r]))
               # #poly = ax.Polygon(zip(xx,yy))
               ax.add_patch(polygon)
               r+=1


                        
                
            
