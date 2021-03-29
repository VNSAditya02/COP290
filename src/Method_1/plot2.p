set datafile separator ","

set terminal png size 700,500

set output 'ErrorVsRuntime_1.png'

set title 'Error Vs Runtime'

set xlabel 'Runtime'

set ylabel 'RMSE'

plot 'ErrorVsRuntime_1.csv' using 1:2 with lines title '' linetype 1 linewidth 3