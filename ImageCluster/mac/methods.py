def get_xy(data):
    xx = []
    yy = []

    
    for p in xrange(data.size()):
        h = data[p]
        xx.append(h.x)
        yy.append(h.y)

    return (xx,yy)
