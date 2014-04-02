<table>
   <tr>
      <td>关于gprof</td>
   </tr>
   <tr>
      <td> 
       1、进入gmon文件夹，运行start.py脚本</br>
       2、单独运行python start.py 生成整张图</br>
3、运行python start.py func_name 生成单个函数相关调用图</br>
   注：修改数据路径和vmlinux的路径、确认虚拟机安装了graphviz</br>
4、测试数据addr_result-3.txt、vmlinux-32在dropbox/kernel/mtrace/data中</br>
  </td>
  </tr>
关于binutils-2.21.1
最好使用ubuntu-12.04-i386.iso的版本
确认安装了flex和bison软件包
安装过程：
1、mkdir binutils-build
2、cd binutils-build
3、../binutils-2.21.1/configure  --prefix=/tools --disable-nls
4、make
5、make install
