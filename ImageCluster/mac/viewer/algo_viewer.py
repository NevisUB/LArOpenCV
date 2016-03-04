from viewer_methods import get_xy, get_xy_w_offset

class AlgoViewer :
    def __init__ (self,algo,algo2,plt):
        self.algo  = algo
        self.algo2 = algo2
        
        self.name = algo.Name()
        self.plt  = plt
        
    def draw(self,ax,cluster,cindex):
        
        if self.name in ["sbc"] :
            return

        if self.name in ["Trackshower"] :
            
            xs = cluster.Origin().y
            ys = cluster.Origin().x
            
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
            
            
        if self.name in ["pcas","pizerofilter"] :

            xs = cluster.Origin().y
            ys = cluster.Origin().x
            
            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,xs,ys)
            ax.plot(px,py,'o',markersize=1,color='black')

            
            if cindex >= self.algo2._pcapaths.size():
                cindex = self.algo2._pcapaths.size() - 1
                
            path   = self.algo2._pcapaths[cindex];

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
            
            ax.plot (xs+cluster._vertex_2D.x,
                     ys+cluster._vertex_2D.y,'o',markersize=10,color='black')
            
            ax.plot([cluster._startPt.x + xs,xs+cluster._vertex_2D.x],
                    [cluster._startPt.y + ys,ys+cluster._vertex_2D.y],'-o',color='pink',lw=3)
            
