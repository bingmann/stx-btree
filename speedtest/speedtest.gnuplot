#!/usr/bin/env gnuplot

set terminal pdf
set output 'speedtest.pdf'

# for generating smaller images:
# set terminal pdf size 4, 2.4

### 1st Plot

set title "Speed Test Multiset - Absolute Time - Insertion Only (125-8000 Items)"
set key top left
set logscale x
set xrange [100:10000]
set xlabel "Inserts"
set ylabel "Seconds"
set format x "%.0f"

plot "speed-insert.txt" using 1:2 title "std::multiset" with linespoints pointtype 2, \
     "speed-insert.txt" using 1:3 title "btree_multiset<4>" with linespoints pointtype 3,  \
     "speed-insert.txt" using 1:31 title "btree_multiset<32>" with linespoints pointtype 4,  \
     "speed-insert.txt" using 1:63 title "btree_multiset<64>" with linespoints pointtype 5, \
     "speed-insert.txt" using 1:129 title "btree_multiset<128>" with linespoints pointtype 6, \
     "speed-insert.txt" using 1:201 title "btree_multiset<200>" with linespoints pointtype 7	

### 2nd Plot

set title "Speed Test Multiset - Absolute Time - Insertion Only (16000-4096000 Items)"

set xrange [10000:5000000]

replot

### 3rd Plot

set title "Speed Test Multiset - Normalized Time - Insertion Only (125-4096000 Items)"
set key top left
set logscale x
set xrange [100:5000000]
set xlabel "Inserts"
set ylabel "Microseconds / Insert"
set format x "%.0f"

plot "speed-insert.txt" using 1:($2 / $1) * 1000000 title "std::multiset" with linespoints pointtype 2, \
     "speed-insert.txt" using 1:($3 / $1) * 1000000 title "btree_multiset<4>" with linespoints pointtype 3,  \
     "speed-insert.txt" using 1:($31 / $1) * 1000000 title "btree_multiset<32>" with linespoints pointtype 4,  \
     "speed-insert.txt" using 1:($63 / $1) * 1000000 title "btree_multiset<64>" with linespoints pointtype 5, \
     "speed-insert.txt" using 1:($129 / $1) * 1000000 title "btree_multiset<128>" with linespoints pointtype 6, \
     "speed-insert.txt" using 1:($201 / $1) * 1000000 title "btree_multiset<200>" with linespoints pointtype 7	

### 4th Plot

set title "Speed Test - Finding the Best Slot Size - Insertion Only - Plotted by Leaf/Inner Slots in B+ Tree"

set key top right
set autoscale x
set xlabel "Leaf/Inner Slots"
set ylabel "Seconds"
unset logscale x
unset logscale y

plot "speed-insert.trt" using ($0 + 4):14 title "1024000 Inserts" with lines, \
     "speed-insert.trt" using ($0 + 4):15 title "2048000 Inserts" with lines, \
     "speed-insert.trt" using ($0 + 4):16 title "4096000 Inserts" with lines

### Now Measuring a Sequence of Insert/Find/Erase Operations

### 1st Plot

set title "Speed Test Multiset - Insert/Find/Erase (125-8000 Items)"
set key top left
set logscale x
set xrange [100:10000]
set xlabel "Data Pairs"
set ylabel "Seconds"
set format x "%.0f"

plot "speed-all.txt" using 1:2 title "std::multiset" with linespoints pointtype 2, \
     "speed-all.txt" using 1:3 title "btree_multiset<4>" with linespoints pointtype 3,  \
     "speed-all.txt" using 1:31 title "btree_multiset<32>" with linespoints pointtype 4,  \
     "speed-all.txt" using 1:63 title "btree_multiset<64>" with linespoints pointtype 5, \
     "speed-all.txt" using 1:129 title "btree_multiset<128>" with linespoints pointtype 6, \
     "speed-all.txt" using 1:201 title "btree_multiset<200>" with linespoints pointtype 7	

### 2nd Plot

set title "Speed Test Multiset - Insert/Find/Erase (16000-4096000 Items)"

set xrange [10000:5000000]

replot

### 3rd Plot

set title "Speed Test Multiset - Normalized Time - Insert/Find/Erase Only (125-4096000 Items)"
set key top left
set logscale x
set xrange [100:5000000]
set xlabel "Items"
set ylabel "Microseconds / Item"
set format x "%.0f"

plot "speed-all.txt" using 1:($2 / $1) * 1000000 title "std::multiset" with linespoints pointtype 2, \
     "speed-all.txt" using 1:($3 / $1) * 1000000 title "btree_multiset<4>" with linespoints pointtype 3,  \
     "speed-all.txt" using 1:($31 / $1) * 1000000 title "btree_multiset<32>" with linespoints pointtype 4,  \
     "speed-all.txt" using 1:($63 / $1) * 1000000 title "btree_multiset<64>" with linespoints pointtype 5, \
     "speed-all.txt" using 1:($129 / $1) * 1000000 title "btree_multiset<128>" with linespoints pointtype 6, \
     "speed-all.txt" using 1:($201 / $1) * 1000000 title "btree_multiset<200>" with linespoints pointtype 7	

### 4th Plot

set title "Speed Test - Finding the Best Slot Size - Insert/Find/Erase - Plotted by Leaf/Inner Slots in B+ Tree"

set key top right
set autoscale x
set xlabel "Leaf/Inner Slots"
set ylabel "Seconds"
unset logscale x
unset logscale y

plot "speed-all.trt" using ($0 + 4):14 title "1024000 Data Pairs" with lines, \
     "speed-all.trt" using ($0 + 4):15 title "2048000 Data Pairs" with lines, \
     "speed-all.trt" using ($0 + 4):16 title "4096000 Data Pairs" with lines

