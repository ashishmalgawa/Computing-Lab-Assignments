"""Ashish Malgawa 17CS60R81 Assignment 1:find_clique"""
import sys
import itertools
import pygraphviz as pgv #install via :pip install pygraphviz
def main():
    """main function"""
    graph = pgv.AGraph()
    input = open(sys.argv[1], "r").read()
    #get filename from command line, open input file and transfer its content in an input variable
    lines = input.split("\n")
    lines[0] = lines[0].rstrip()
    lines[0] = lines[0].lstrip()
    k = int(lines[0]) #get value of k
    del lines[0] #removing k form input
    if k <= len(lines):
        blacklist = []
        #this list will contain elements who are not connected to
        #more than k-1 elements.Since, if they do not have k-1 edges
        #associated with them then they cannot be a part of a k-length clique
        temp_adj = []
        vertices = []
        for line in lines:
            line = line.rstrip()
            line = line.lstrip()
            if len(line) != 0:
                temp = line.split(" ")
                vertices.append(temp[0])
                graph.add_node(temp[0])
                for node in temp[1:]:
                    if node not in vertices:
                        graph.add_edge(temp[0], node)
                temp_adj.append(temp)
        while True:
            temp_adj = sorted(temp_adj, key=len)
            i = 0
            for line in temp_adj:
                if (len(line)-1) >= k-1:
                    break
                i += 1
            p = 0
            if i == 0:
                break
            if len(temp_adj) < k:
                print "There are no cliques in this graph with k=", k
                graph.write("file.dot")
                graph.draw('graph_image.png', prog="dot")
                sys.exit()
            while p < i:
                blacklist.append(temp_adj[0][0])
                del temp_adj[0]
                p += 1
            iterator1 = 0
            while iterator1 < len(temp_adj):
                iterator2 = 0
                while iterator2 < len(temp_adj[iterator1]):
                    if temp_adj[iterator1][iterator2] in blacklist:
                        del temp_adj[iterator1][iterator2]
                    iterator2 += 1
                iterator1 += 1
            iterator1 = 0
            while iterator1 < len(vertices):
                if vertices[iterator1] in blacklist:
                    del vertices[iterator1]
                else:
                    iterator1 += 1
            del blacklist[:]
        adjacency = {}
        i = 0
        for line in temp_adj:
            adjacency[line[0]] = line
            del adjacency[line[0]][0]
        cliques = []
        combinations = list(itertools.combinations(vertices, k)) #finding all combinations
        for c in combinations: #applying  brute force search on rest of the elements
            notclique = False
            for elem in c:
                notclique = False
                count = 0
                for vertice in adjacency[elem]:
                    #checking if all vertices are connected to each other
                    if vertice in c:
                        count += 1
                if count < k-1:
                    #if even one of them is not  connected to rest of the vertices
                    #in combination then it is not a clique
                    notclique = True
                    break
            if notclique:
                continue
            else:
                cliques.append(c)
        flag = 1
        if len(cliques) == 0:
            print "There are no cliques in this graph with k=", k
        for clique in cliques:
            print ' '.join(clique)
            if flag == 1:
                flag = 0
                for elem in clique:
                    graph.get_node(elem).attr['color'] = 'green'
                    graph.get_node(elem).attr['style'] = 'filled'
        graph.write("file.dot")
        graph.draw('graph_image.png', prog="dot")
    else:
        print "k cannot be greater than n"
        graph.write("file.dot")
        graph.draw('graph_image.png', prog="dot")
if __name__ == '__main__':
    main()
