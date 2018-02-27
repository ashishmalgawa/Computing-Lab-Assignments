'''This Programme Takes Random Number And Genrates given no. of triangles. It also finds the incircle and calculates the area of maximum cirlce possible. At the end it generates a parallelogram parallel to y-axis and having the minimum area that covers all the triangles.'''

import sys
import random
import math
import graphviz as gv
from subprocess import call

#Graphviz variable Graph_detail contains all details regarding the graph

GRAPH_DETAIL = gv.Graph(format='dot')
GRAPH_DETAIL.node('inv_node', style='invis', pos='0, 0!')
GRAPH_DETAIL.node('inv_node_corner', style='invis', pos='1600, 1600!')
GRAPH_DETAIL.edge('inv_node', 'inv_node_corner', style='invis')
n = 0


class Triangle:
    '''Takes two random co-ordinate generated in range of 400-1200 and calculates the position of third vertex accordingly. In case cant generte than again takes random number till all the three vertices generated. Also calcultes the area of triangle, circle'''
    
    #Constructor Initializes all the triangle co-ordinates and calculates the co-ordinate of third vertex. Calculates Triangle area, Circle Area
    #Also contains all geters to get value of all varibles of triangle class
    def __init__(self, x1, y1, x2, y2,):
        self.x1 = x1
        self.x2 = x2
        self.x3 = -1
        self.y1 = y1
        self.y2 = y2
        self.y3 = -1;
        s60 = math.sin((60 * math.pi)/180)
        c60 = math.cos((60 * math.pi)/180)
        self.tx3 = c60 * (self.x1 - self.x2) - s60 * (self.y1 - self.y2) + self.x2
        self.ty3 = s60 * (self.x1 - self.x2) + c60 * (self.y1 - self.y2) + self.y2
        self.tx4 = c60 * (self.x1 - self.x2) + s60 * (self.y1 - self.y2) + self.x2
        self.ty4 = -1*s60 * (self.x1 - self.x2) + c60 * (self.y1 - self.y2) + self.y2
        while(self.x3<400 or self.y3<400 or self.x3>1200 or self.y3>1200):
            if self.tx3>400 and self.ty3>400 and self.tx3<1200 and self.ty3<1200:
                self.x3 = self.tx3;
                self.y3 = self.ty3;
            elif self.tx4>400 and self.ty4>400 and self.tx4<1200 and self.ty4<1200:
                self.x3 = self.tx4;
                self.y3 = self.ty4;
            else:
                self.x1 = random.uniform(400, 1200)
                self.x2 = random.uniform(400, 1200)
                self.y1 = random.uniform(400, 1200)
                self.y2 = random.uniform(400, 1200)
                self.tx3 = c60 * (self.x1 - self.x2) - s60 * (self.y1 - self.y2) + self.x2
                self.ty3 = s60 * (self.x1 - self.x2) + c60 * (self.y1 - self.y2) + self.y2
                self.tx4 = c60 * (self.x1 - self.x2) + s60 * (self.y1 - self.y2) + self.x2
                self.ty4 = -1*s60 * (self.x1 - self.x2) + c60 * (self.y1 - self.y2) + self.y2
        self.side_length = math.sqrt((self.x2-self.x1)**2 + (self.y2-self.y1)**2)
        self.triangle_area = ((math.sqrt(3))*(self.side_length)*self.side_length)/4
        self.circle_radius = (math.sqrt(3)*self.side_length)/6
        self.circle_x = ((self.x1*self.side_length) + (self.x2*self.side_length) + (self.x3*self.side_length))/(3*(self.side_length))
        self.circle_y = ((self.y1*self.side_length) + (self.y2*self.side_length) + (self.y3*self.side_length))/(3*(self.side_length))
        self.circle_area = (math.pi)*(self.circle_radius)*(self.circle_radius)
    
    #returns triangle area
    def get_triangle_area(self):
        return self.triangle_area
    
    #returs triangle side length
    def get_side_length(self):
        return self.side_length
    
    #returns x1 co-ordinate
    def get_x1(self):
        return self.x1
    
    #returns x2 co-ordinate of triangle
    def get_x2(self):
        return self.x2
    
    #returns x3 co-ordinate of triangle
    def get_x3(self):
        return self.x3
    
    #returns y1 co-ordinate of triangle
    def get_y1(self):
        return self.y1
    
    #returns y2 co-ordinate of triangle
    def get_y2(self):
        return self.y2
    
    #returns y3 co-ordinate of triangle
    def get_y3(self):
        return self.y3
    
    #returns x cordinate of center of circle
    def get_circle_x(self):
        return self.circle_x
    
    #returns y cordinate of center of circle
    def get_circle_y(self):
        return self.circle_y
    
    #returns radius of circle
    def get_circle_radius(self):
        return self.circle_radius
    
    #returns area of circle
    def get_cirlce_area(self):
        return self.circle_area
    
def make_Triangle(x, *args):
    '''Make all the triangles in the graph'''
    s1 = str(x.get_x1()) + "," + str(x.get_y1()) + "!"
    GRAPH_DETAIL.node('a'+ str(x), label="", shape="point", pos=s1)
    s1 = str(x.get_x2()) + "," + str(x.get_y2()) + "!"
    GRAPH_DETAIL.node('b' + str(x), label="", shape="point", pos=s1)
    s1 = str(x.get_x3()) + "," + str(x.get_y3()) + "!"
    GRAPH_DETAIL.node('c' + str(x), label="", shape="point", pos=s1)
    GRAPH_DETAIL.edge('a' + str(x), 'b' + str(x))
    GRAPH_DETAIL.edge('b' + str(x), 'c' + str(x))
    GRAPH_DETAIL.edge('c' + str(x), 'a' + str(x))
    
def make_Circle(x, *args):
    '''make all the circles'''
    s1 = str(x.get_circle_x()) + ',' + str(x.get_circle_y()) + '!'
    GRAPH_DETAIL.node('c1' + str(x), label="", shape='circle', pos = s1, width=str(x.get_circle_radius()/36.4), color='green')

class Point2D(object): #convex hull source : 
    #References
    #https://github.com/berkaykicanaoglu/Convex-Hull-using-Monotone-Chain/blob/master/convex-//hull.py
    # constructor for the point class
    def __init__(self, x, y):
        self.x = x
        self.y = y
    # override the representation method   
    def __repr__(self):
        return "(%f, %f)" %(self.x, self.y) 
    def getCoordinates(self):
        return [self.x, self.y]    
class Line2D(object):
    # constructor for the line class
    def __init__(self, point1, point2):
        self.p1 = point1
        self.p2 = point2
        # calculate the slope of the line
        self.slope = (self.p2.y - self.p1.y) \
                     / (self.p2.x - self.p1.x) 
        # set the variable derivative
        if self.slope > 0:
            self.derivative = "positive"
        elif self.slope == 0:
            self.derivative = "none"
        else:
            self.derivative = "negative" 
    # return the slope
    def getSlope(self):
        return self.slope
    def orientation(self,line2):
        # get slopes
        slope1 = self.getSlope()
        slope2 = line2.getSlope()
        # get difference
        diff = slope2-slope1
        # determine orientation
        if diff > 0:
            return -1 # counter-clockwise
        elif diff == 0:
            return 0  # colinear
        else:
            return 1  # clockwise 
#----------------------------
# Helper functions
#----------------------------
def getX(point):
    return point.x
def getY(point):
    return point.y            
def sortPoints(point_list):
    point_list = sorted(point_list,key = getX)
    # This function do not account for comparison
    # of two points at the same x-location. Resolve!  
    return point_list          
def ConvexHull(point_list):
    # initalize two empty lists for upper 
    # and lower hulls.
    upperHull = []
    lowerHull = [] 
    # sort the list of 2D-points
    sorted_list = sortPoints(point_list) 
    for point in sorted_list:
        if len(lowerHull)>=2:
            line1 = Line2D(lowerHull[len(lowerHull)-2],\
                           lowerHull[len(lowerHull)-1]) 
            line2 = Line2D(lowerHull[len(lowerHull)-1],\
                           point)               
        while len(lowerHull)>=2 and \
        line1.orientation(line2) != -1:
            removed = lowerHull.pop()
            if lowerHull[0] == lowerHull[len(lowerHull)-1]:
                break
            # set the last two lines in lowerHull
            line1 = Line2D(lowerHull[len(lowerHull)-2],\
                           lowerHull[len(lowerHull)-1]) 
            line2 = Line2D(lowerHull[len(lowerHull)-1],\
                           point)
        lowerHull.append(point)  
    # reverse the list for upperHull search     
    reverse_list = sorted_list[::-1]                    
    for point in reverse_list:
        if len(upperHull)>=2:
            line1 = Line2D(upperHull[len(upperHull)-2],\
                           upperHull[len(upperHull)-1]) 
            line2 = Line2D(upperHull[len(upperHull)-1],\
                           point)               
        while len(upperHull)>=2 and \
        line1.orientation(line2) != -1:
            removed = upperHull.pop()
            if upperHull[0] == upperHull[len(upperHull)-1]:
                break
            # set the last two lines in lowerHull
            line1 = Line2D(upperHull[len(upperHull)-2],\
                           upperHull[len(upperHull)-1]) 
            line2 = Line2D(upperHull[len(upperHull)-1],\
                           point)
        upperHull.append(point)          
    # final touch: remove the last members
    # of each point as they are the same as 
    # the first point of the complementary set. 
    removed = upperHull.pop()
    removed = lowerHull.pop()
    # concatenate lists
    convexHullPoints ={
        "lowerHull" : lowerHull ,
        "upperHull":upperHull}
    return convexHullPoints
def main():
    FILE1 = open(sys.argv[1], "r")
    for x in FILE1:
        n = int(x)
    collection = []
    #Random co-ordinate generation of n triangles
    for i in range(0, n):
        t1 = Triangle(random.uniform(400, 1200), random.uniform(400, 1200), random.uniform(400, 1200), random.uniform(400, 1200))
        collection.append(t1)
    
    for x in collection:
        make_Triangle(x)
        make_Circle(x)
        print x.get_x1(), x.get_y1(), x.get_x2(), x.get_y2(), x.get_x3(), x.get_y3(), x.get_triangle_area(), x.get_circle_x(), x.get_circle_y(), x.get_cirlce_area(), x.get_circle_radius()
    
    # Read the points from the vertices of random triangles
    point_list = []
    for x in collection:
        point = Point2D(x.get_x1(),x.get_y1())
        point_list.append(point)
        point = Point2D(x.get_x2(),x.get_y2())
        point_list.append(point)
        point = Point2D(x.get_x3(),x.get_y3())
        point_list.append(point)
    # call the convex hull (Monotone Chain) function
    convexSet_list = ConvexHull(point_list)
    min_x=getX(convexSet_list['lowerHull'][0]);
    max_x=getX(convexSet_list['lowerHull'][0]);    
    for point in set(convexSet_list['lowerHull']+convexSet_list['upperHull']):
        if getX(point) < min_x :
            min_x=getX(point)
        if getX(point) > max_x :
            max_x=getX(point)
    best_para={
        "area":1600*1600,
        "points":[]
    }
    new_list=convexSet_list['lowerHull']+convexSet_list['upperHull']
    #intialised with max area possible
    for i in range(len(new_list)):
            if (i==len(new_list)-1): #taking adjacent edges
                x1=getX(new_list[i])
                y1=getY(new_list[i])
                x2=getX(new_list[0])
                y2=getY(new_list[0])
            else:
                x1=getX(new_list[i])
                y1=getY(new_list[i])
                x2=getX(new_list[i+1])
                y2=getY(new_list[i+1])
            b = x2 - x1 #ax +by +c=0
            a = y1 - y2
            c = - 1 * (a * x1 +b * y1)
            farthest_point = new_list[0];
            distance=abs(a*getX(new_list[0]) + b * getY(new_list[0]) + c)
            for point in range(1,len(new_list)):
                if distance < abs(a*getX(new_list[point]) + b * getY(new_list[point]) + c): #this is not actual distance I have not divided
                    farthest_point = new_list[point];                                       #with square_root(a*a+b*b) since it is constant
                    distance = abs(a*getX(new_list[point]) + b * getY(new_list[point]) + c)
            c2 = -1 *(a * getX(farthest_point) + b * getY(farthest_point))
            
            #Parallel Line passing from the farthest point
            py1 = (-c - a * min_x)/b 
            py2 = (-c - a * max_x)/b 
            py3 = (-c2 - a * max_x)/b 
            py4 = (-c2 - a * min_x)/b 
            area = (abs(min_x * (py2-py4) + max_x * (py4-py1) + min_x * (py1-py2))+abs(min_x * (py2-py3) + max_x * (py3-py4) + max_x * (py4-py2)))/2
            if best_para['area'] > area:
                best_para['area']=area
                best_para['points']=[(min_x,py1),(max_x,py2),(max_x,py3),(min_x,py4)] 
    px1 = best_para['points'][0][0]
    py1 = best_para['points'][0][1]
    s = str(px1) + "," + str(py1) + "!"
    GRAPH_DETAIL.node('parraleogram_a', label="", shape="point", pos=s)
    px2 = best_para['points'][1][0]
    py2 = best_para['points'][1][1]
    s = str(px2) + "," + str(py2) + "!"
    GRAPH_DETAIL.node('parraleogram_b', label="", shape="point", pos=s)   
    px3 = best_para['points'][2][0]
    py3 = best_para['points'][2][1]
    s = str(px3) + "," + str(py3) + "!"
    GRAPH_DETAIL.node('parraleogram_c', label="", shape="point", pos=s)
    px4 = best_para['points'][3][0]
    py4= best_para['points'][3][1]
    print px1, py1, px2, py2, px3, py3, px4, py4,best_para["area"]
    s = str(px4) + "," + str(py4) + "!"
    GRAPH_DETAIL.node('parraleogram_d', label="", shape="point", pos=s)
    GRAPH_DETAIL.edge('parraleogram_a', 'parraleogram_b', color='red')
    GRAPH_DETAIL.edge('parraleogram_b', 'parraleogram_c', color='red')
    GRAPH_DETAIL.edge('parraleogram_c', 'parraleogram_d', color='red')
    GRAPH_DETAIL.edge('parraleogram_d', 'parraleogram_a', color='red')
    file_output = open('outputGraph.dot', 'w')
    file_output.write(str(GRAPH_DETAIL))
    file_output.close()
    call(["neato","-n", "-Tpng", "outputGraph.dot", "-o", "graph_image.png"])

if __name__=="__main__":
    main()