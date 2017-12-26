# ----------------------------------------------------------
# HW 11
# ----------------------------------------------------------
# Please answer these questions after having completed the 
# entire assignment.
# ----------------------------------------------------------
# Name:  Jacob Feldman
# Hours spent in total:  4
# Collaborators (if any) and resources used (if any): Jackson Jacoby 
# This assignment tries to connect what you've learned in
# in this course with the real world, by guiding you the
# process of building a small but real web search engine. 
# What did you think of this assignment?
#   I think its really cool, i wish we had done more of these
# Feedback: What was the hardest part of this assignment?
#   just debugging all the functions
# Feedback: Any suggestions for improving the assignment?   
#   go slower with explaining searching html links
# ----------------------------------------------------------
import urllib2
import random
def download_page(pagename):
    ''' takes the name of a page and returns the page contents in the form of a string'''
    fullurl = "http://cs.colgate.edu/cosc101/testweb/" + pagename  #creates new file object 
    fileobject = urllib2.urlopen(fullurl)
    
    return fileobject.read()        #reads file object


def extract_links(htmlcontents):
    '''takes the contents of an html link and returns the links imbeded in it in the form of a list'''
    linklist=[]
    index=0 
    while index>=0:
        index=htmlcontents.find("<a href=",index)       #looks for the <a> tag and then finds the link within it
        if index >=0: 
            endquote = htmlcontents.find('"',index+9)
            pagename=htmlcontents[(index+9):endquote] 
            linklist.append(pagename)
            index=endquote
    return linklist 



def remove_tags(htmlcontents):
    '''accepts the contents of an html link and returns the page string without any tags'''
    newstring=''
    a=0
    htmlcontents=htmlcontents.split('<')    #looks for the open tag and splits on that index
    for string in htmlcontents:
        for I in range(len(string)):
            if string[I]== '>':             #deletes anything before the >
                a=I
        newstring+=string[a+1:]        
                                                                                   
    return newstring       

def normalize_word(string):
    '''takes a single word and returns it without any unusual charecters in the form of a string'''
    string=string.strip()           #gets rid of new line charecters or anything at the beginning or end of the string
    new_string=''
    for char in string:             #checks if it is a letter and accumulates it lower case
        if char.isalpha():
            new_string+=char.lower()
    return new_string

def index_page(pagename,pagecontents,reverseindex):
    '''takes the name of the page, the string of
    words in the page, and the reverse index and returns None, simply edits the reverse index dictoinary adding the links where the word appears
    '''
    pagecontents = pagecontents.split() #splits the contents into single words in a list
    for word in pagecontents:
        word = normalize_word(word)
        if word not in reverseindex.keys():  #creates a dictionary key value pair
            reverseindex[word] = [pagename]
        elif pagename not in reverseindex[word]:
            reverseindex[word] += [pagename]
    


def crawl_web(page_name, reverse_index, webgraph):
    '''takes the name of a starter page, the reverse_index and the webgraph and returns None. Follows links and creates a list of paths between links'''
    page_contents = download_page(page_name)
    links = extract_links(page_contents)
    index_page(page_name, page_contents, reverse_index)
    webgraph[page_name] = links
    for outlink in links:             
        if outlink not in webgraph:
            #print "visiting",outlink,"from",page_name
            crawl_web(outlink, reverse_index, webgraph)    
    return webgraph

def random_surfer_simulation(web_graph,p,simulations):
    '''takes the web graph, a probability, and a number of simluations to run and returns a dictionary of visited links. randomly surfs through links in the web graph based on probability p'''
    currpage = random.choice(web_graph.keys())
    d={}
    count=0
    while count<simulations:
        num=random.random()
        if num < (p):                   #if the random number is less than p, randomly choose a link within the page
            currpage = random.choice(web_graph.keys())
            if currpage in d:
                d[currpage]+=1.0/simulations
            else:
                d[currpage]=1.0/simulations
        if num > (p):                   #if the random number is greater than p, randomly choose any link from all possible links
            currpage = random.choice(web_graph[currpage])
            if currpage in d:
                d[currpage]+=1.0/simulations
            else:
                d[currpage]=1.0/simulations
        count+=1
    return d

def list_union(list1,list2):
    '''combines two lists without repeats takes two lists and returns a new combined list'''
    newlist=[]                  
    for item in list1+list2:
        if item not in newlist:
            newlist+= [item]
    return newlist

def list_intersection(listone,listtwo):
    '''takes two lists and returns a list that contains terms that appear in both'''
    newlist=[]
    for item in listone:
        if item in listtwo:
            if item not in newlist:
                newlist +=[item]
    return newlist

def list_difference(list1,list2):
    '''takes two lists and returns a list that contains terms that appear in one and not in the other'''
    newlist=[]
    for item in list1:
        if item not in list2:
            newlist.append(item)
    return newlist

def get_query_hits(word,reverse_index):
    '''takes a search term and returns the links where that word exists in the form of a list'''
    if word in reverse_index.keys():
        return reverse_index[word]
    else:
        return []

def print_ranked_results(results,page_rank):
    '''takes the results of the search and the page ranks and prints an ordered list of results'''
    ranks = []
    i = 1
    if len(results) == 0:
        print 'No matches for search terms.'
        return
    
    #print ranks
    for item in results:
        ranks+=[page_rank[item]]        #accumulates a new list of ranks for sorting
    ranks=sorted(ranks, reverse=True)   #sorts the ranks
    for rank1 in ranks:                 #goes back through the ranks and finds the matches for the ordered terms
        for rank in page_rank:
            if page_rank[rank]==rank1:
                print str(i)+':',rank , "(rank:", str(rank1)+")"        #final output lines
        i += 1



def process_query(query_string, reverse_index):
    '''takes a query string and the reverse index and returns a list of matches for the term in the form of a list
    '''
    do_intersection = False
    terms = query_string.split()
    if terms[0] == 'AND':
        do_intersection = True
        terms.pop(0)
    includes = []
    excludes = []
    for term in terms:
        if term[0] == '-':
            excludes.append(normalize_word(term))
        else:
            includes.append(normalize_word(term))
    if includes==[]:
        return []

    matches=get_query_hits(includes[0],reverse_index)
    
    for term in includes:
        if term in reverse_index:   
            hits = reverse_index[term]
            if do_intersection:
                matches = list_intersection(matches, hits)
            else:
                matches = list_union(matches, hits)
    for term in excludes:
        if term in reverse_index:
            hits = reverse_index[term]
            matches = list_difference(matches, hits)
    return matches



def search_engine(reverse_index,page_ranks):
    '''takes the reverse index and the page_rank dictionary and runs the search process. Returns None'''
    query=''
    results={}
    while query != "DONE":
        query=raw_input("Search terms? (enter 'DONE' to quit) ")
        if query == "DONE":
            break
        results=process_query(query,reverse_index)
        print_ranked_results(results,page_ranks)
    return
        
def main():
    reverse_index={}
    webgraph={}
    crawl_web('a.html',reverse_index,webgraph)
    page_ranks=random_surfer_simulation(webgraph,.15,100000)
    search_engine(reverse_index,page_ranks)
main()
    
