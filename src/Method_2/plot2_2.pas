set datafile separator ","

set terminal png size 700,500

set output 'ErrorVsNoOfPixels.png'

set title 'Error Vs No Of Pixels'

set xlabel 'No of Pixels'

set ylabel 'RMSE'

plot 'ErrorVsNoOfPixels.csv' using 1:2 with lines title '' linetype 1 linewidth 3