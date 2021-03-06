from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from nltk.stem import *
from nltk.stem.porter import *
import sys
from nltk.tokenize import sent_tokenize
import math 
import numpy as np #for matrices
from io import open #for encoding 
import time
import datetime
DAMPING_FACTOR = .85 #damping factor
stemmer = PorterStemmer()
stop_words = set(stopwords.words('english'))
input = open("reader/"+sys.argv[1]+".txt","r", encoding='utf-8').read() #input original text file
HASHTAGS = sys.argv[2].split("#")[1:]
i = 0
sent_tokenize_list = input.split("\n") #extracting sentences in form of list
GOLD_SUMMARY_LENGTH = 10 #number of sentences in summary
punctuations=['(',')',';',':','!','@','{','}','[',']','\'','\"',',','.','#',"https","..","..."] #to remove punctuations and useless words
processed_sent_list=[] #sentences after removing stop words,stemming and case folding
lala = True
output=sys.argv[1]+"\t"+datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')+"\n"
for hasht in HASHTAGS:
    sentence_with_hashtags=[]
    processed_sent_list=[]
    for sentence in sent_tokenize_list:
        word_tokens = word_tokenize(sentence) #list of words in a sentence 
        flag = False
        for i in range(len(word_tokens)):
            if word_tokens[i]==unicode("#"):
                if word_tokens[i+1].lower() == hasht.lower():
                    sentence_with_hashtags.append(sentence)
    k =0
    for sentence in sentence_with_hashtags:
        word_tokens = word_tokenize(sentence) #list of words in a sentence 
        filtered_sentence=[] #temporary variable
        i=0; #counter
        while i <len(word_tokens):
            if i!=0 and word_tokens[i-1]!="." and word_tokens[i][0].isupper(): #to check if it is a proper noun 
                filtered_sentence.append(word_tokens[i])
            else :
                if word_tokens[i].lower() not in stop_words and word_tokens[i] not in punctuations:   
                    filtered_sentence.append(stemmer.stem(word_tokens[i].lower())) #stemming ,removing stop words and case folding
            i +=1
        processed_sent_list.append(filtered_sentence)
        
    output+=unicode("#"+hasht+"\n")
    #checking if there are no sentences with the current hashtag
    if len(processed_sent_list)==0:
        continue
    def pageRank(adjacency_matrix):
        i = 0 #counter
        page_rank_list=[] #page rank list
        out_sum=[]
        while i < len(processed_sent_list):
            j = 0 #counter
            sum = 0 #sum 
            while j < len(processed_sent_list):
                sum += adjacency_matrix[i][j] #sum of weight of all the out edges
                j += 1
            sum /= DAMPING_FACTOR #because an element contains weight * DAMPING_FACTOR
            out_sum.append(sum)
            i += 1
        i = 0
        while i < len(processed_sent_list):
            j = 0
            while j < len(processed_sent_list): #traversing vertices with incoming edges to current vertex
                if out_sum[j] != 0:
                    adjacency_matrix[j][i]  = adjacency_matrix[j][i] / float(out_sum[j]) 
                    #dividing s*d by sum of the weight of edges of current incoming vertex
                j += 1
            i += 1
            n=len(processed_sent_list)
            page_rank_list.append(1/float(n)) #default value of page rank
        super_matrix = np.matrix(adjacency_matrix).transpose() #EACH ELEMENT WILL HAVE SIMILARITY * DAMPING FACTOR/SUM OF OUT EDGES
        page_rank= np.matrix(page_rank_list).transpose() #creating transpose
        while True:
            new_page_rank=super_matrix * page_rank + 1 - DAMPING_FACTOR #applying page rank formulae
            if np.allclose(new_page_rank,page_rank):
                break;
            page_rank = new_page_rank #converge when the current page rank and earlier page rank is same
        return page_rank
    def summaryMaker(b):
        top = [] #this will contain a tuple sentence number,rank of that sentence
        i = 0
        #page rank in list
        for rank in b.tolist():
            if len(top) >= GOLD_SUMMARY_LENGTH:
                j = 0
                while j < len(top):
                    if rank[0]>top[j][0]: #if current rank is greater than the rank of any top sentence
                        temp=list(top[j])
                        top[j][0]=rank[0]
                        top[j][1]=i
                        j += 1
                        while j < len(top): #then this loop will swap all the sentences till the end and we will
                            temp2=list(top[j]) # remove the sentence at the last index
                            top[j]=list(temp)
                            temp=list(temp2)
                            j += 1
                    j += 1
            else:
                top.append([rank[0],i]) #first add sentences till GOLD_SUMMARY_LENGTH is fulfilled
                top.sort(key = lambda x:x[0],reverse=True) #then sort by their page ranks
            i += 1 
        summary =[] #Now we will create an empty summary list
        for element in top:
            summary.append(sentence_with_hashtags[element[1]]) #add original sentences whose indices are in top list
        return '\n'.join(summary) #join them to create the summary
    forward = []
    backward = []
    for i in range(len(processed_sent_list)):
        j = 0
        forward_row = []
        back_row=[]
        while j <=  i:
            forward_row.append(0) #appending 0 in the row of forward adjacency matrix
            if j != i:
                num = 0 #This will store intersection between two sentences
                for word in processed_sent_list[i]:
                    if word in processed_sent_list[j]:
                        num +=1 #calculating intersection
                den = math.log(len(processed_sent_list[i]))+math.log(len(processed_sent_list[j]))
                s=num/den  #calculating similarity score
                back_row.append(s * DAMPING_FACTOR) #adding the edge weight in adjacency matrix  
            else :
                back_row.append(0) #when both the vertices are same there is no edge
            j += 1
            #after this j will be equal to i+1
        while j < len(processed_sent_list):
            num = 0 #This will store intersection between two sentences
            for word in processed_sent_list[i]:
                if word in processed_sent_list[j]:
                    num +=1 #calculating intersection
            den = math.log(len(processed_sent_list[i]))+math.log(len(processed_sent_list[j]))
            s=num/den #calculating similarity score
            forward_row.append(s * DAMPING_FACTOR) #adding s*damping factor in the row
            back_row.append(0)
            j += 1
        forward.append(forward_row) #adding the row in the forward adjacency matrix
        backward.append(back_row)   #adding the row in the forward adjacency matrix
    #a = pageRank(forward)
    b = pageRank(backward)
    #x = summaryMaker(a) #x will contain summary in string form of forward matrix
    y = summaryMaker(b) #y will contain summary in string form of backward matrix
    output+=y+"\n"
output+="==============================================================================="    
open("output/"+sys.argv[1]+".txt","w").write(output)
