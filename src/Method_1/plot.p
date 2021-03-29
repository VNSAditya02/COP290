set datafile separator ","

set terminal png size 700,500

set output 'Queuensity.png'

set title 'QueueDensity'

set xlabel 'time(in sec)'

set ylabel 'QueueDensity'

plot 'Method1_data.csv' using 1:2 with lines title 'n = 1' linetype 1 linewidth 3, '' using 1:3 with lines title 'n = 3' linetype 2 linewidth 3, '' using 1:4 with lines title 'n = 5' linetype 3 linewidth 3, '' using 1:5 with lines title 'n = 7' linetype 4 linewidth 3, '' using 1:6 with lines title 'n = 10' linetype 5 linewidth 3, '' using 1:7 with lines title 'n = 15' linetype 6 linewidth 3, '' using 1:8 with lines title 'n = 20' linetype 7 linewidth 3, '' using 1:9 with lines title 'n = 30' linetype 8 linewidth 3,'' using 1:10 with lines title 'n = 40' linetype 9 linewidth 3,'' using 1:11 with lines title 'n = 50' linetype 10 linewidth 3