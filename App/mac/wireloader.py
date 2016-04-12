import ROOT

def load(fname):

    vv = []
    
    inside = None
    with open(fname,'r') as f:
        inside = f.read()

    inside = inside.split("\n")

    for ix,p in enumerate(inside):

        if len(p) != 1:
            continue

        v = ROOT.std.vector(ROOT.std.pair(int,int))()

        wires = inside[ix+1].split(",")

        for wire in wires:

            if wire == '': continue

            wire = wire.split(" ")
            w = ROOT.std.pair(int,int)(int(wire[0]),int(wire[1]))
            v.push_back(w)
        
        vv.append(v)
    return vv
    
