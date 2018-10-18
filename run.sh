#!/bin/bash
ls -l
make clean
make all
ls -l

./tcpserver&

sleep 1

./tcpclient <<"EOF"
localhost
20987
c
0
c
1
d
1
200
w
1
200
w
0
200
t
1
300
t
1
200
w
0
150
w
0
160
q
y
localhost
20987
w
0
40
c
0
c
1
q
n
EOF

pkill tcpserver
