set datafile separator ","

set terminal png size 700,500

set output 'Error Vs Runtime'

set title 'Error Vs Runtime'

set xlabel 'Runtime'

set ylabel 'RMSE'

plot 'Method_4_Data.csv' using 2:3 with lines title '' linetype 1 linewidth 3