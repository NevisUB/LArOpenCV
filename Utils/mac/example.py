import sys
from ROOT import imutil

k=imutil.ContourTreeMaker()

#
# Fill 3 contours using ContourTreeMaker::FillContour function.
# The 1st argument is the contour index (id) number, 2nd is the x-coordinate,
# and 3rd is the y-coordinte to be appended to the specified contour.
# If contour of a specified index does not exist, it will be created
# automatically (which means you don't have to start filling from 0).
#

print
print 'Filling contours (will print values filled)'
print

# Fill 2nd contour with meaningless numbers
contour_index = 2
print 'Contour',contour_index,'...',
for x in xrange(10):
    x_value,y_value=(float(x%3),float(x%3))
    print '(%.2g,%.2g)' % (x_value,y_value),
    k.FillContour(contour_index, x_value, y_value)
print

# Fill 1st contour with meaningless numbers
contour_index = 1
print 'Contour',contour_index,'...',
for x in xrange(10):
    x_value,y_value=(float(x%2),float(x%2))
    print '(%.2g,%.2g)' % (x_value,y_value),
    k.FillContour(contour_index, x_value, y_value)
print

# Fill 0th contour with meaningless numbers
contour_index = 0
print 'Contour',contour_index,'...',
for x in xrange(10):
    x_value,y_value=(float(x%1),float(x%1))
    print '(%.2g,%.2g)' % (x_value,y_value),
    k.FillContour(contour_index, x_value, y_value)
print

# Fill TTree
print 'Filling TTree now...'
print
k.Fill()

# Loop over Tree contents to make sure it's filled
tree = k.Tree()
print 'Looping over recorded TTree entries (%d entry)' % tree.GetEntries()
print

for entry in xrange(tree.GetEntries()):

    # Read-in entry into memory
    tree.GetEntry(entry)

    # Get a ContourArray_t pointer in memory
    contour_array = tree.contours

    # Loop over contours
    for contour_index in xrange(contour_array.size()):

        print 'Contour',contour_index,'...',

        contour = contour_array[contour_index]

        # Loop over coordinates
        for coordinate_index in xrange(contour.size()):

            coordinate = contour_array[contour_index][coordinate_index]

            print '(%.2g,%.2g)' % (coordinate.first,coordinate.second),

        # Insert a new line
        print

print
            
