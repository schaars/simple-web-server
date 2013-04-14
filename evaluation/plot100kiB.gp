set term jpeg enhanced
set output "plot100kiB.jpeg"

set xlabel "Number of clients"
set ylabel "Throughput (req/s)"
set xrange [1:256]
set yrange [0:]
set grid
set key right bottom

plot '-' using 1:2 title "Rust" with linespoints, '-' using 1:2 title "C" with linespoints, '-' using 1:2 title "C w/ sendfile()" with linespoints
#nbClients  Rust
1	111
16	848
160	887
240	941
256	960
e
#nbClients  C
1	125
16	838
80	1040
160	1040	
256	1040
e
#nbClients  C-sendfile
1	127
16	832
80	1065	
256	1065
