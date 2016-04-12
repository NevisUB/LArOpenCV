import sys

inside = None
with open(sys.argv[1],'r') as f:
    inside = f.read()
    
inside = inside.split("\n")

dwf = open("dead_wires.fcl","w+")

dwf.write("DeadWireCombine: {\n")

pp = -1
for ix,p in enumerate(inside):
    
    if len(p) != 1:
        continue
    pp+=1
    
    wires = inside[ix+1].split(",")

    left  = []
    right = []
    for wire in wires:

        if wire == '': continue
        
        wire = wire.split(" ")
        left.append(wire[0])
        right.append(wire[1])

    dwf.write("Plane{}Left: [ ".format(pp))
    for ix,l in enumerate(left) :

        if ix == len(left) - 1:
            dwf.write(l);
            break
        
        dwf.write("{},".format(l))

    dwf.write("]\n");

    dwf.write("Plane{}Right: [ ".format(pp))
    for ix,r in enumerate(right) :

        if ix == len(right) - 1:
            dwf.write(r);
            break
        
        dwf.write("{},".format(r))
    dwf.write("]\n");

dwf.write("}\n");
dwf.close()
