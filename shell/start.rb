`make`
`./edit`
`gprof vmlinux-32 gmon.out >gmon.txt`
`python gprof2dot.py gmon.txt>gmon.dot`
`dot -Tsvg gmon.dot>gmon.svg`
