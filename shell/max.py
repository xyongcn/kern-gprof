#!/usr/bin/env python
import os
#name="/home/tanh/gmon/trace.txt"
name="/home/tanh/gmon/addr_result-1.txt"
name1="/home/tanh/gmon/trace_resault.txt"
m=""
n=""
l=[]
def  delect(fp):
     for line in fp:
          data=line.split(",");
         # l.append(int(data[2].strip()))
          l.append(data[0])
          l.append(data[1])
     print max(l),min(l)
     #print a
if __name__=="__main__":
   fp=open(name,"r")
   
   delect(fp)
