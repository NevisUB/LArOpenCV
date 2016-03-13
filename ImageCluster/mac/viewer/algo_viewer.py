from viewer_methods import get_xy, get_xy_w_offset

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

        if self.name in ["gs"] :
            
            insidehits = cluster._insideHits

            px, py     = get_xy_w_offset(insidehits,xs,ys)
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
	      pts_x.append( cluster.PolyObject.Point(pt).first + xs)
	      pts_y.append( cluster.PolyObject.Point(pt).second + ys) 
	     # ax.plot(cluster.PolyObject.Point(pt).first + xs, cluster.PolyObject.Point(pt).second + ys,'o',color='red',markersize=10)

	    pts_x.append( cluster.PolyObject.Point(0).first + xs)
	    pts_y.append( cluster.PolyObject.Point(0).second + ys) 

	    ax.plot(pts_x,pts_y,color='red',lw=2)
            
	    #Plot start + end
            ax.plot(cluster._startPt.x + xs,
                    cluster._startPt.y + ys,'o',color='green',markersize=10)

            ax.plot(cluster._endPt.x + xs,
                    cluster._endPt.y + ys,'o',color='pink',markersize=5)
            
            
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

            # ax.plot([cluster._m[0].x + xs,cluster._m[1].x + xs],
            #         [cluster._m[0].y + ys,cluster._m[1].y + ys],'-',color='green',lw=5)

            insidehits = cluster._insideHits
            px, py     = get_xy_w_offset(insidehits,xs,ys)
            ax.plot(px,py,'o',markersize=1,color='black')

            # ch = cluster._chosen

            # ax.plot([ch[0].x+xs,ch[1].x+xs,ch[2].x+xs,ch[3].x+xs,ch[0].x+xs],
            #         [ch[0].y+ys,ch[1].y+ys,ch[2].y+ys,ch[3].y+ys,ch[0].y+ys],color='magenta',lw=5)
            # 

            # ax.plot(cluster._startPt.x + xs,
            #         cluster._startPt.y + ys,'o',color='pink',markersize=10)
            
            
            nboxes = cluster._verts.size();
            print "nboxes",nboxes
            for v in xrange(nboxes):
                box = cluster._verts[v]

                ax.plot([box[0].x+xs,box[1].x+xs,box[2].x+xs,box[3].x+xs,box[0].x+xs],
                        [box[0].y+ys,box[1].y+ys,box[2].y+ys,box[3].y+ys,box[0].y+ys],lw=2)



                        
                
