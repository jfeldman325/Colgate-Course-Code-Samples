# ----------------------------------------------------------
# HW 10
# ----------------------------------------------------------
# Please answer these questions after having completed the 
# entire assignment.
# ----------------------------------------------------------
# Name: Jacob Feldman
# Hours spent in total:  
# Collaborators (if any) and resources used (if any):  
# Feedback: getting the input output to work properly
# Feedback: Any suggestions for improving the assignment?   
#   nope
# ----------------------------------------------------------

def caesar_encode(write,text, key):
    # write your caesar cipher encoding here
    '''takes a string and a numerical key and returns an encoded string, uses caesar method'''
    encoded=''
    encoded2='' 
    myfile=open(text, 'r')
    newfile=open(write,'w')
    for line in myfile:
        
        for char in line:
            if char.isupper():
                modulo=65
            if char.islower():
                modulo=97
            if char.isalpha():
                num_char= ord(char)     #shifts the text to the right by adding the numerical key
                num_key=(num_char-modulo)+key
                char_modulo= num_key%26
                encoded+=chr(char_modulo+modulo)
            else:
                encoded+=char
        newfile.write(encoded)
        encoded2+=encoded
        encoded=''
    return encoded2
def crack_key(text,write):
    d={}
    max1=0
    text2=''
    myfile=open(text,'r')
    for line in myfile:    
        for char1 in line:
            if char1.isalpha():
                text2+=char1
        for char in text2:
            if char in d:
               d[char]+=1
            else:
                d[char]=1
        max1=max(d.values())
    for count in d:  #traces back the year that the max was achieved
        if d[count]== max(d.values()):
             maximum=count
    if ord(maximum)>=97:
        key=ord(maximum)-ord('e')
    else:
        key=ord(maximum)-ord('E')
    caesar_encode(write,text,26-key)
    return key
        
def vigenere_encode(write, text,key):
    # write your vigenere cipher encoding here
    '''takes a string and a string text and returns an encoded string, uses vigenere method'''
    encoded=''
    I=0
    encoded2='' 
    myfile=open(text, 'r')
    newfile=open(write,'w')
    for line in myfile:
        for char in line:       
            if char.isupper():
                x=65
            if char.islower():
                x=97                                             #if its a lower case letter
            if char.isalpha():
                num_char=(ord(char)-x)+(ord(key[I])-65)              #mapps the letter to a number starting at 0, then adds the value of the key
                num_modulo=num_char%26
                encoded+=chr(num_modulo+x)                           #converts the value back to a charecter
                I+=1
                if I>=len(key):
                    I=0   
            elif not char.isalpha():    #if its not a letter at all
                encoded+=char
        newfile.write(encoded)
        encoded2+=encoded
        encoded=''
def rotate_key(key):
    '''takes a string and rotates the charecters for use in vignere cipher decoding'''
    new_key=''
    for char in key:
        num_char=(ord(char)-65)         
        num_modulo=26-(num_char%26)
        new_key+=chr(num_modulo+65)
    return new_key

def main():
    pass
    choice_one=raw_input('Do you want to(c)ipher or(d)ecipher or (cr)ack? ')
    choice_two=raw_input('Which cipher (c)easer or (v)igenere? ')
    text= raw_input('Name of file to read: ')
    write= raw_input('Name of file to write: ')
    if choice_one=='c' and choice_two=='c':
        key=raw_input('Enter key: ')
        caesar_encode(write,text,int(key))
        print 'I wrote the encoded text to',write
    elif choice_one=='d' and choice_two=='c':
        key=raw_input('Enter key: ')
        caesar_encode(write,text,26-(int(key)))
        print 'I wrote the decoded text to',write
    elif choice_one=='c' and choice_two=='v':
        key=raw_input('Enter key: ')
        vigenere_encode(write,text,key)
        print 'I wrote the encoded text to',write
    elif choice_one=='d' and choice_two=='v':
        key=raw_input('Enter key: ')
        vigenere_encode(write,text,rotate_key(key))
        print 'I wrote the decoded text to',write
    elif choice_one=='cr' and choice_two=='c':
        print "I cracked the code! The key is", crack_key(text,write),"I wrote the deciphered text to", write
    elif choice_one=='cr' and choice_two=='v':
        print "I don't crack Viginere code!"
    
# please keep these next two lines as-is. 
# they will call the main function when
# your program is run.
if __name__ == '__main__':
    main()
