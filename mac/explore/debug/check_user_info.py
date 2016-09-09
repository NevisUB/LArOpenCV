#get larlite
from larlite import larlite as ll
from larlite import larutil as lu

geo=lu.Geometry.GetME()
geoH=lu.GeometryHelper.GetME()

import ROOT
import root_numpy as rn
import pandas as pd
    
#graphics 
import numpy as np
import matplotlib
import matplotlib.pyplot as plt


#create storage manager
sm = ll.storage_manager()
sm.set_io_mode(ll.storage_manager.kREAD)
FILENAME="../../../App/mac/mctester_output.root"
sm.add_in_filename(FILENAME)
sm.open()

for evt in xrange(10):
    print "~~~~~~~~~~~> event is %d <~~~~~~~~~~~"%evt
    sm.go_to(evt)

    #get hits and clusters
    hitprod='gaushit'
    event_hits     = sm.get_data(ll.data.kHit,"%s"%hitprod)
    event_clusters = sm.get_data(ll.data.kCluster,"ImageClusterHit")

    #get ass
    event_ass      = sm.get_data(ll.data.kAssociation,"ImageClusterHit")

    #get association
    cluster_to_hit_ass = event_ass.association(event_clusters.id(),event_hits.id())

    #get the vertex_data
    VTXPRODUCER='mcvertex'
    event_vertex = sm.get_data(ll.data.kVertex,VTXPRODUCER)

    #get contour data
    event_user = sm.get_data(ll.data.kUserInfo,"ImageClusterHit")
        

    gausdata = rn.root2array(FILENAME,
                            treename='hit_%s_tree'%hitprod,
                            branches=['hit_%s_branch.vector<larlite::hit>.fPeakTime'%hitprod,
                                      'hit_%s_branch.vector<larlite::hit>.fWireID.Wire'%hitprod,
                                      'hit_%s_branch.vector<larlite::hit>.fWireID.Plane'%hitprod])

    hits_df= pd.DataFrame({'Wire'      : gausdata[evt][1],
                           'PeakTime'  : gausdata[evt][0],
                           'Plane'     : gausdata[evt][2]})


    #Make sure vars are initialized
    for j in xrange(3):
        exec("plane%d=None"%j)
        exec("sbcplane%d=None"%j)
        exec("houghplane%d=None"%j)
        exec("hullplane%d=None"%j)
        exec("roiplane%d=None"%j)
        exec("flashplane%d=None"%j)
        
    for s in xrange(event_user.size()):
        uinfo=event_user[s]
        id_ = uinfo.get_string("ID")
        
        if id_ == "Algo_pc_Plane_2_clusters" : sbcplane0 = uinfo
        if id_ == "Algo_pc_Plane_1_clusters" : sbcplane1 = uinfo
        if id_ == "Algo_pc_Plane_2_clusters" : sbcplane2 = uinfo

        if id_ == "Algo_cwo_Plane_0_clusters" : plane0 = uinfo
        if id_ == "Algo_cwo_Plane_1_clusters" : plane1 = uinfo
        if id_ == "Algo_cwo_Plane_2_clusters" : plane2 = uinfo

        if id_ == "Algo_hpts_Plane_0_clusters" : houghplane0 = uinfo
        if id_ == "Algo_hpts_Plane_1_clusters" : houghplane1 = uinfo
        if id_ == "Algo_hpts_Plane_2_clusters" : houghplane2 = uinfo

        if id_ == "Algo_ac_Plane_0_clusters" : hullplane0 = uinfo
        if id_ == "Algo_ac_Plane_1_clusters" : hullplane1 = uinfo
        if id_ == "Algo_ac_Plane_2_clusters" : hullplane2 = uinfo
                    
        if id_ == "Algo_cwo_Plane_0_roi_bounds" : roiplane0 = uinfo
        if id_ == "Algo_cwo_Plane_1_roi_bounds" : roiplane1 = uinfo
        if id_ == "Algo_cwo_Plane_2_roi_bounds" : roiplane2 = uinfo

        if id_ == "Algo_fm_Plane_0_clusters" : flashplane0 = uinfo
        if id_ == "Algo_fm_Plane_1_clusters" : flashplane1 = uinfo
        if id_ == "Algo_fm_Plane_2_clusters" : flashplane2 = uinfo
        
        
    #Show the ImageClusters
    for plane in [0,1,2]:
        sbc_data=event_user[plane]
        hsv = plt.get_cmap('jet')
        lors = hsv(np.linspace(0, 1.0, cluster_to_hit_ass.size()))

        fig,ax=plt.subplots(figsize=(10*2,6*2))

        phits = hits_df.query("Plane=={}".format(plane))

        ax.plot(phits.Wire,phits.PeakTime,'o',color='black',markersize=3,alpha=0.2)

        exec("sbc_data = sbcplane%s"%plane)

        if sbc_data :
            nsbc_clusters = sbc_data.get_int("NClusters")
        
            for c in xrange(nsbc_clusters):
    
                xx=np.array(sbc_data.get_darray('ClusterID_{}_contour_x'.format(c)))
                yy=np.array(sbc_data.get_darray('ClusterID_{}_contour_y'.format(c)))
                xx=np.append(xx,xx[0])
                yy=np.append(yy,yy[0])

                plt.plot(yy,xx,':',lw=2,color='Grey')
        
        # plot each cluster and userinfo
        for ix,cluster in enumerate(cluster_to_hit_ass):

            ##############################
            #get the hits for this clusters
            slice_=np.array(cluster)
            if event_clusters[ix].Plane().Plane != plane: continue
            if slice_.size == 0: continue

            hslice = hits_df.iloc[slice_]
            ax.plot(hslice.Wire,hslice.PeakTime,'o',color='black',markersize=3)

            ##############################
            #call user_info get_darray, for the contour   
            exec("cluster_data = plane%s"%plane)

            cid = event_clusters[ix].ID()
            xx=np.array(cluster_data.get_darray('ClusterID_{}_contour_x'.format(cid)))
            yy=np.array(cluster_data.get_darray('ClusterID_{}_contour_y'.format(cid)))
            xx=np.append(xx,xx[0])
            yy=np.append(yy,yy[0])

            plt.plot(yy,xx,'-',lw=2)

            ##############################
            #put the hull info on
            exec("hull_data = hullplane%s"%plane)
            if hull_data:
                xx=np.array(hull_data.get_darray('ClusterID_{}_hull_x'.format(cid)))
                yy=np.array(hull_data.get_darray('ClusterID_{}_hull_y'.format(cid)))
    
                if xx.size > 1:
                    xx=np.append(xx,xx[0])
                    yy=np.append(yy,yy[0])

                    plt.plot(yy,xx,'-',lw=2,color='red')
            
            ##############################
            #put vertex information on screen as a big star 

            vtx=ROOT.std.vector('double')()
            vtx.resize(3)
            vtx[0]=event_vertex[0].X()
            vtx[1]=event_vertex[0].Y()
            vtx[2]=event_vertex[0].Z()
            px = geoH.Point_3Dto2D(vtx.data(),plane)
            pt=np.array([px.w/geoH.WireToCm(),px.t/geoH.TimeToCm()+800])
            print "EVENT VERTEX IS ",pt
            ax.plot(pt[0],pt[1],'*',markersize=20,color='yellow')

            ##############################
            #can we overlay the start points?
            startx = cluster_data.get_double("ClusterID_{}_startPt_x".format(cid))
            starty = cluster_data.get_double("ClusterID_{}_startPt_y".format(cid))
            ax.plot(starty,startx,'*',markersize=20,color='orange')

            #can we overlay the end points
            endx = cluster_data.get_double("ClusterID_{}_endPt_x".format(cid))
            endy = cluster_data.get_double("ClusterID_{}_endPt_y".format(cid))
            ax.plot(endy,endx,'*',markersize=20,color='green')

            #can we put the eigenvect first on there?
            eigenx = cluster_data.get_double("ClusterID_{}_eigenVecFirst_x".format(cid))
            eigeny = cluster_data.get_double("ClusterID_{}_eigenVecFirst_y".format(cid))

            #how long should it be?
            len_ = np.sqrt(np.power((endy-starty),2) + np.power((endx-startx),2))
            print "len_ {}".format(len_)
            print  "eigenx {} eigeny {}".format(eigenx,eigeny)
            #normalize it
            size_ = np.sqrt(np.power(eigeny,2) + np.power(eigenx,2))
            eigenx/=size_
            eigeny/=size_

            t=50
            ex=t*eigenx*6 #6==meta.pixel_height()
            ey=t*eigeny
            print "ex {} ey {} size_ {}".format(ex,ey,size_)
            ax.plot([starty-ey,starty,starty+ey],[startx-ex,startx,startx+ex],'-',lw=3,color='orange')

            #get the angle, lets make the line accounding to this guy
            angle=cluster_data.get_double("ClusterID_{}_angle".format(cid))

            ey=50*np.cos(angle)
            ex=50*np.sin(angle)
            ax.plot([starty-ey,starty,starty+ey],[startx-ex,startx,startx+ex],'-',lw=4,alpha=0.5,color='cyan')
            
            
            
        exec("roi_data = roiplane%s"%plane)  
        ##############################
        #put the bounding box in

        bbx=np.array(roi_data.get_darray('roi_bounds_x'))
        bby=np.array(roi_data.get_darray('roi_bounds_y'))

        bbx=np.append(bbx,bbx[0])
        bby=np.append(bby,bby[0])
        plt.plot(bbx,bby,'-',lw=2,color='brown')

        # ax.set_xlim(bbx.min()-25,bbx.max()+25)
        # ax.set_ylim(bby.min()-25,bby.max()+25)

        ##############################
        #can we overlay the hough lines?
        exec("houghdata = houghplane%s"%plane)

        if houghdata:
            nlines=houghdata.get_int('N_lines')
            
            for n in xrange(nlines):
                x1=houghdata.get_double("line_{}_x1".format(n))
                y1=houghdata.get_double("line_{}_y1".format(n))
                x2=houghdata.get_double("line_{}_x2".format(n))
                y2=houghdata.get_double("line_{}_y2".format(n))
                ax.plot([y1,y2],[x1,x2],'--',lw=3,color='yellow')


        ##############################
        #lets put on there my flashlights
        exec("flash_data = flashplane%s"%plane)

        if flash_data:
            for i in xrange(flash_data.get_int("N_hulls")):
                xx=np.array(flash_data.get_darray('ClusterID_{}_contour_x'.format(i)))
                yy=np.array(flash_data.get_darray('ClusterID_{}_contour_y'.format(i)))
                xx=np.append(xx,xx[0])
                yy=np.append(yy,yy[0])
                
                plt.plot(yy,xx,'-',lw=2,color='blue',alpha=0.5)

        plt.grid()
        ax.set_title("Event {} Plane {}".format(evt,plane),fontweight='bold',fontsize=30)
        # ax.set_aspect('equal')
        plt.savefig("event_{}_plane_{}.png".format(evt,plane))
        
        plt.close()


