`readelf -s vmlinux-32 1>readelf.txt`
`nm -l vmlinux-32 1>nm.txt`
#require 'sqlite3'
#db=SQLite3::Database.new('test.db')
#db.escute("create table addre(arr varchar(20),startline varchar(100),endaddr(20),endline varchar(100))")
fh=File.new("result-1.txt","w")  
File.open("readelf.txt","r") do |file|  
	while line = file.gets  
		arr=line.split(" ")
		if(arr[3]=="FUNC")
			tmp=`grep #{arr[1]} nm.txt | grep -w #{arr[7]}`
			w=tmp.split(" ")
			startline=w[3]
			#startline=`addr2line "0x#{arr[1]}" -e vmlinux`		
			endaddr="0x"+("0x#{arr[1]}".to_i(16)+arr[2].to_i(10)-1).to_s(16)	
			endline=`addr2line "#{endaddr}" -e vmlinux-32`
			#puts startline 开始行号
			#puts endaddr   结束地址
			#puts endline   结束行号
			#arr[1]         开始地址
			#arr[7]         函数名 			
			#`sqlite3 analysis.db 'insert into funcinfo values("0x#{arr[1]}","#{startline}","#{endaddr}","#{endline}","#{arr[7]}");'`           
			#db.execute("INSERT INTO address (arr,startline,endaddr,endline) VALUES(\"#{arr[1]}","#{startline}","#{endaddr}","#{endline}","#{arr[7]}\")")
                       # fh.puts "#{arr[1]}"+" "+"#{startline}".chomp+" "+"#{endaddr}"+" "+"#{endline}".chomp+" "+"#{arr[7]}"
                       fh.puts "#{endaddr}"+" "+"#{arr[7]}".chomp
		end
      end  
end  
