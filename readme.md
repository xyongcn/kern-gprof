����gprof
@������gmon�ļ��У�����start.py�ű�
2����������python start.py ��������ͼ
3������python start.py func_name ���ɵ����������ص���ͼ
   ע���޸�����·����vmlinux��·����ȷ����������װ��graphviz
4����������addr_result-3.txt��vmlinux-32��dropbox/kernel/mtrace/data��

����binutils-2.21.1
����ʹ��ubuntu-12.04-i386.iso�İ汾
ȷ�ϰ�װ��flex��bison������
��װ���̣�
1��mkdir binutils-build
2��cd binutils-build
3��../binutils-2.21.1/configure  --prefix=/tools --disable-nls
4��make
5��make install
