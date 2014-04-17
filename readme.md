<table>
   <tr>
      <td>关于gprof</td>
   </tr>
   <tr>
      <td> 
       1、进入gmon文件夹，运行start.py脚本<br/>
       2、单独运行python start.py 生成整张图<br/>
3、运行python start.py func_name 生成单个函数相关调用图<br/>
   注：修改数据路径和vmlinux的路径、确认虚拟机安装了graphviz<br/>
4、测试数据addr_result-3.txt、vmlinux-32在dropbox/kernel/mtrace/data中<br/>
  </td>
  </tr>
  </table>
<table>
  <tr>
    <td>关于binutils-2.21.1</td>
  </tr>
  <tr>
      <td>
      最好使用ubuntu-12.04-i386.iso的32位版本<br/>
      如果使用ubuntu-12.04-amd64.iso的64位版本修改如下:<br/>
            makefile文件中的cc 编译添加-m32选项<br/>
            sudo apt-get install libc6:i386<br/>
            sudo apt-get install ia32-libs<br/>
      确认安装了flex和bison软件包<br/>
      安装过程：<br/>
      1、mkdir binutils-build<br/>
      2、cd binutils-build<br/>
      3、../binutils-2.21.1/configure  --prefix=/tools --disable-nls<br/>
      4、make<br/>
      5、make install<br/>
      6、cd /usr/bin<br/>
      7、rm gprof<br/>
      8、ln -s /tools/bin/gprof gprof <br/>
      </td>
 </tr>
</table>
