import math


def main():

    user_input=""
    points=[]
    while user_input != "done":
        point = input("Enter a point seperated by a comma EX: 3,4 and press ENTER, type done when finished:")
        if point=="done":
            break
        else:
            points.append(point.split(","))
    output=[]
    for point in points:
        templist=[int(point[0]),int(point[1])]
        output.append(templist)
    sortedx=output.copy()
    sortedy=output.copy()
    sortedx.sort(key=takeFirst)
    sortedy.sort(key=takeSecond)
    print(closest_point(sortedx,sortedy))
    
   
    

def takeFirst(x):
    return x[0]

def takeSecond(x):
    return x[1]
    
def dist(p1,p2):
    return math.sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+((p1[1]-p2[1])*(p1[1]-p2[1])))

        
    
def brute_force(Points):
    p1=Points[0]
    p2=Points[1]
    ln_Points=len(Points)
    
    minimum=dist(p1,p2)
    if ln_Points==2:
        print(p1)
        print(p2)
        return p1,p2,minimum
    else:
        for i in range(0,ln_Points-1):
            for j in range(i+1, ln_Points):
                if i!=0 and j!=1:
                    distance=dist(Points[i],Points[j])
                    if distance<minimum:
                        minimum=distance
                        p1,p2 = Points[i], Points[j]

    return p1,p2,minimum
            
        
def closest_point(P_L,P_R):

    ln_PL=len(P_L)
    ln_PR=len(P_R)
    
    if ln_PL<=3:
        return brute_force(P_L)
    if ln_PR<=3:
        return brute_force(P_R)
    
    X_L=P_L[:int(ln_PL/2)]
    X_R=P_L[int(ln_PL/2):]
    X_Center_Val=P_L[int(ln_PL/2)][0]
    Y_L=[]
    Y_R=[]
    for point in P_R:
        if point[0]<X_Center_Val:
            Y_L.append(point)
        else:
            Y_R.append(point)
            
    p1_L,p2_L,distL = closest_point(X_L,Y_L)
    p1_R,p2_R,distR = closest_point(X_R,Y_R)

    min_dist=0.0
    min_pair=[]
    if distL<distR:
        min_dist,min_pair=distL,[p1_L,p2_L]
       # print(min_pair)
    else:
        min_dist,min_pair=distR,[p1_R,p2_R]
        #print(min_pair)

    in_strip=[]
    for point in P_L:
        #print(abs( point[0]-X_Center_Val))
        #print(min_dist)
        if abs( point[0]-X_Center_Val)< min_dist:
            in_strip.append(point)
    #print(in_strip)
    in_strip_ln=len(in_strip)
    for i in range(0,in_strip_ln-1): 
        for j in range(i+1,min(7,in_strip_ln)):
            print(in_strip[i],in_strip[j])
    
            dist_ij=dist(in_strip[i],in_strip[j])
            if dist_ij<min_dist:
                min_dist=dist_ij
                min_pair=[in_strip[i],in_strip[j]]
    return min_dist,min_pair
            
main()
