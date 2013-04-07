set term jpeg enhanced
set output "plot100kiB.jpeg"

set xlabel "Number of clients"
set ylabel "Throughput (kreq/s)"
set xrange [1:256]
set yrange [0:]
set grid
set key right bottom

plot '-' using 1:($2/1000) title "Rust" with linespoints, '-' using 1:($2/1000) title "C" with linespoints, '-' using 1:($2/1000) title "C w/ sendfile()" with linespoints
#nbClients  Rust
1	214
16	1445
48 3824
64	3890
128 3878
e
#nbClients  C
1	140
16	1036
64	1030
128	1006
e
#nbClients  C-sendfile
1  224
16	1603
64	6112
128	11947
192	13543
256	13426
