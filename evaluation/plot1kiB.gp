set term jpeg enhanced
set output "plot1KiB.jpeg"

set xlabel "Number of clients"
set ylabel "Throughput (req/s)"
set xrange [1:224]
set yrange [0:]
set grid

plot '-' using 1:2 title "Rust" with linespoints, '-' using 1:2 title "C" with linespoints
#nbClients  Rust
1     207 
16	   1465
32	   2876
48	   3555
64	   3596
80	   3643
96	   3641
112	3642
224	3578
e
#nbClients  C
1     227
16	   1491
32	   2964
48	   4286
64	   5655
80	   6779
96	   8198
112	8705
128	9361
144	9889
160	10776
176	11334
192	11792
208	12263
224	12113

