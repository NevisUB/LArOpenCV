import numpy as np

def get_bounded_line(lline,re):
    
    slope  = ( lline[3] - lline[1] ) / ( lline[2] - lline[0] )
    yinter = lline[1]
    
    xx = np.arange(lline[0],lline[2],0.1)
    yy = ( xx - re.x )*slope + yinter
    
    xx = xx.tolist()
    yy = yy.tolist()
    
    xes = [x for x in xx if yy[ xx.index(x) ] >= re.y and yy[ xx.index(x) ] <= (re.y + re.height) ]
    yes = [y for y in yy if y >= re.y and y <= (re.y + re.height)]

    xes = np.array(xes)
    yes = np.array(yes)

    return (xes,yes)
