def get_xy(data) :
    xx = []
    yy = []

    for p in xrange(data.size()):
        h = data[p]
        xx.append(h.x)
        yy.append(h.y)

    return (xx,yy)

def get_xy_w_offset(data,x,y) :
    xx = []
    yy = []

    for p in xrange(data.size()):
        h = data[p]
        xx.append(h.x + x)
        yy.append(h.y + y)

    return (xx,yy)

def draw_cluster(ax,cluster) :

    contour = cluster._contour
    
    if contour.size() == 0:
        print "CLUSTER OF ZERO SIZE FOUND!"
        return

    if contour.size() > 100000:
        print "CLUSTER CONTOUR WAY TOO BIG"
        return
        
    cx,cy = get_xy_w_offset(contour,cluster.Origin().y,cluster.Origin().x)

    cx.append(cx[0])
    cy.append(cy[0])
        
    ax.plot(cx,cy,'-',lw=3)
