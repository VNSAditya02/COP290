set datafile separator ","

set terminal png size 700,500

set output 'Queuensity.png'

set title 'QueueDensity'

set xlabel 'time(in sec)'

set ylabel 'QueueDensity'

plot 'Method2_data.csv' using 1:2 with lines title '338 x 778' linetype 1 linewidth 3, '' using 1:3 with lines title '300 x 600' linetype 2 linewidth 3, '' using 1:4 with lines title '200 x 400' linetype 3 linewidth 3, '' using 1:5 with lines title '150 x 200' linetype 4 linewidth 3, '' using 1:6 with lines title '100 x 100' linetype 5 linewidth 3, '' using 1:7 with lines title '50 x 100' linetype 6 linewidth 3, '' using 1:8 with lines title '50 x 50' linetype 7 linewidth 3