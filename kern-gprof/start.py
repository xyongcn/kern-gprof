#!/usr/bin/env python
import os
import sys
#name="/home/tanh/md5-busybox-data-thui.txt"
#name1="/home/tanh/data/result-1.txt"
name2=os.getcwd()+"/addr_result-2.txt"
l=[]
def  addr(fp):
     fp1=open(name2,"w")
    # ap=open(name1,"r")
   
     for line in fp:
         addr_data=line.split(" ");
         addr_4=addr_data[4].split(":")[1].strip()
         addr_5=addr_data[5].split(":")[1].strip()
         ap=open(name1,"r")
         i=1
         j=1
         for line1 in ap:
             data=line1.split(" ")
            # print addr_data[0].strip()
             
             if str(data[1]).strip()==str(addr_4).strip() and i==1:  
                           
                 st=data[0]
                 i=0

                # continue
             elif data[1].strip()==addr_5.strip() and j==1:
                  st1=data[0]
                  j=0
             elif i==0 and j==0:
                  fp1.write(st)
                  fp1.write(",")
                  fp1.write(st1+","+addr_data[7].split(":")[1].strip()+os.linesep)
                 
                  break
                 # continue 
def  gprof():
     fp1=open(name2,"r")
     for line in fp1:
          data=line.split(",");
         # l.append(int(data[2].strip()))
          l.append(data[0])
          l.append(data[1])
     #return l
     if os.system("make") ==0:
       print "make successful" 
     else:
       print "error"
     if  os.system("./edit "+" "+max(l)+" "+min(l)) == 0:
        print "successful installed" 
     else:
       print "error"+max(l)
     if len(sys.argv) ==1:
        if os.system("gprof vmlinux-32 gmon.out >gmon.txt") ==0:
          print "successfully analysis " 
        else:
          print "error"
     else:
        if os.system("gprof -f"+ sys.argv[1]+" vmlinux-32 gmon.out >gmon.txt") ==0:
          print "successfully analysis" 
        else:
          print "error"
     if os.system("python gprof2dot.py -n0 -e0 gmon.txt >gmon.dot") ==0:
       print "make successful" 
     else:
       print "error"
     if os.system("dot -Tsvg  gmon.dot >gmon.svg") ==0:
       print "successful drawed " 
     else:
       print "error"
                  

    
   
             
if __name__=="__main__":
  # fp=open(name,"r")  
   #addr(fp)
   gprof()
   
