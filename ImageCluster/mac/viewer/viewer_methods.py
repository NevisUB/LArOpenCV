import numpy as np

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

def get_bounded_line(eline,ecenter,re):


    def rx(t):
        return ecenter.x + eline.x * t

    def ry(t):
        return ecenter.y + eline.y * t

    t1 = ( re.x + re.width - ecenter.x ) / eline.x
    t2 = ( re.x            - ecenter.x ) / eline.x

    a = np.array([[rx(t1),rx(t2)],
                  [ry(t1),ry(t2)]])
    return a
