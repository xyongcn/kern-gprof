#!/usr/bin/env python
import os
#name="/home/tanh/gmon/trace.txt"
name="/home/tanh/md5-busybox-data-thui.txt"
name1="/home/tanh/data/result-1.txt"
name2="/home/tanh/gmon/addr_result-1.txt"
def  addr(fp):
     fp1=open(name2,"w")
     for line in fp:
         addr_data=line.split(" ");
         #print addr_data[3].strip()
         #print addr_data[0].strip()
         addr_4=addr_data[4].split(":")[1].strip()
         addr_5=addr_data[5].split(":")[1].strip()
         ap=open(name1,"r")
         for line1 in ap:
             data=line1.split(" ")
            # print addr_data[0].strip()
             if str(data[1]).strip()==str(addr_4).strip():  
                # print data[1].strip(),addr_data[0].strip()              
                 st=data[0]
                # fp1.write(",")
                 ap=open(name1,"r")
                 for line2 in ap:
                     data=line2.split(" ")
                    
                     if addr_5.strip()=="ed_deschedule" and data[1]=="ed_deschedule":
                          print "aaaaaa"
                     if data[1].strip()==addr_5.strip():
           
                            
                        fp1.write(st)
                        fp1.write(",")
                        fp1.write(data[0]+","+addr_data[7].split(":")[1].strip()+os.linesep)
                        break
                     else:
                        continue
                 
                 break
             else:
                continue
         
if __name__=="__main__":
   fp=open(name,"r")
  # ap=open(name1,"r")
   
   addr(fp)
