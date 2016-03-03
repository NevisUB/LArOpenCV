from viewer_methods import get_xy

class AlgoViewer :
    def __init__ (self,algo,plt):
        self.algo = algo
        self.name = algo.Name()
        self.plt  = plt
        
    def draw(self,ax,cluster,cindex):
        
        if self.name in ["sbc"] :
            return
            
        if self.name in ["pcas"] :

            insidehits = cluster._insideHits
            px, py     = get_xy(insidehits)
            ax.plot(px,py,'o',markersize=1,color='black')
            

            path   = self.algo._pcapaths[cindex];
            nboxes = path.chosen_boxes_.size()

            for b in xrange(nboxes):
                box       = path.chosen_boxes_[b]
                rectangle = self.plt.Rectangle((box.x, box.y),
                                               box.width, box.height,
                                               fc='white',ec='black',alpha=1,lw=3)
                ax.add_patch(rectangle)
                
            bbox = cluster._minAreaRect
            ax.plot([bbox[0].x,bbox[1].x,bbox[2].x,bbox[3].x,bbox[0].x],
                    [bbox[0].y,bbox[1].y,bbox[2].y,bbox[3].y,bbox[0].y],color='orange',lw=2)

