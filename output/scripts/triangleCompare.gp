call "Header.gp" 1028 (768 - 200)

set output "triangleCompare.png"

set multiplot
set xtics nomirror

set key autotitle columnheader
set key off
set grid y
set xtics rotate by -45
set style fill solid 0.7
set boxwidth 0.7
# set facecolor #FFFFFF

set tmargin at screen 0.8
set bmargin at screen 0.2

# set key outside


unset border
# set title "Average time spent initializing" font ",18"

# stats "../vk.a.initialize.2.40.2.txt" using 6

stats "../vk.c.createTrianglesSlow.8.txt" i 0 using 2 nooutput

# set label 3 "Significant driver involvement" at 3, screen 0.64 font ",16"
# set arrow 1 from 3, screen 0.6 to 3, screen 0.5 head size 5.0,2.0 front filled
# set arrow 2 from 6, screen 0.6 to 6, screen 0.5 head size 5.0,2.0 front filled
# set arrow 3 from 7, screen 0.6 to 7, screen 0.5 head size 5.0,2.0 front filled

set object 1 rect from screen 0.5, screen 0.02 to screen 0.55, screen 0.05 fc rgb COLOR_G front
set label 4 "Ignoring idle times" at screen 0.56, screen 0.03 font ",12" left

set border 8
LMARGIN=0.08
XWIDTH=0.18
RMARGIN=0.94

set label 3 "Creating and filling N vertex buffers" at screen 0.5, screen 0.93 font ",20" center

set label 2 "Vulkan" at graph 0.3, graph 0.9 font ",20" center
set title "Host\n(No device involvement)"
set ylabel "Time in seconds" offset 2.5 font ",16"
set lmargin at screen (LMARGIN)
set rmargin at screen (LMARGIN+XWIDTH)
set xrange[-1:4]
set yrange[0:STATS_max*1.3]
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.b.createTrianglesHost.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_V, \
     "../vk.b.createTrianglesHost.2.txt" i 0 using (1):2 with boxplot lc rgb COLOR_V, \
     "../vk.b.createTrianglesHost.4.txt" i 0 using (2):2 with boxplot lc rgb COLOR_V, \
     "../vk.b.createTrianglesHost.8.txt" i 0 using (3):2 with boxplot lc rgb COLOR_V,

unset label 2
set title "\"Wasteful\"\n(Over-synchronization)"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH)
set rmargin at screen (LMARGIN+XWIDTH*2)
set xrange[-1:4]
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.c.createTrianglesSlow.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_V, \
     "../vk.c.createTrianglesSlow.2.txt" i 0 using (1):2 with boxplot lc rgb COLOR_V, \
     "../vk.c.createTrianglesSlow.4.txt" i 0 using (2):2 with boxplot lc rgb COLOR_V, \
     "../vk.c.createTrianglesSlow.8.txt" i 0 using (3):2 with boxplot lc rgb COLOR_V,

set title "\"Smart\"\n(Command daisy-chain)"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH*2)
set rmargin at screen (LMARGIN+XWIDTH*3)
set xrange[-1:4]
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.d.createTrianglesSmart.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_V, \
     "../vk.d.createTrianglesSmart.2.txt" i 0 using (1):2 with boxplot lc rgb COLOR_V, \
     "../vk.d.createTrianglesSmart.4.txt" i 0 using (2):2 with boxplot lc rgb COLOR_V, \
     "../vk.d.createTrianglesSmart.8.txt" i 0 using (3):2 with boxplot lc rgb COLOR_V, #\
    #  "../vk.d.createTrianglesSmart.1.txt" i 0 using (0):3 with boxplot lc rgb COLOR_V, \
    #  "../vk.d.createTrianglesSmart.2.txt" i 0 using (1):3 with boxplot lc rgb COLOR_V, \
    #  "../vk.d.createTrianglesSmart.4.txt" i 0 using (2):3 with boxplot lc rgb COLOR_V, \
    #  "../vk.d.createTrianglesSmart.8.txt" i 0 using (3):3 with boxplot lc rgb COLOR_V, 


set title "\"Fast\"\n(Deferred submission)"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH*3)
set rmargin at screen (LMARGIN+XWIDTH*4)
set xrange[-1:4]
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.e.createTrianglesFast.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_G, \
     "../vk.e.createTrianglesFast.2.txt" i 0 using (1):2 with boxplot lc rgb COLOR_G, \
     "../vk.e.createTrianglesFast.4.txt" i 0 using (2):2 with boxplot lc rgb COLOR_G, \
     "../vk.e.createTrianglesFast.8.txt" i 0 using (3):2 with boxplot lc rgb COLOR_G, \
     "../vk.e.createTrianglesFast.1.txt" i 0 using (0):3 with boxplot lc rgb COLOR_V, \
     "../vk.e.createTrianglesFast.2.txt" i 0 using (1):3 with boxplot lc rgb COLOR_V, \
     "../vk.e.createTrianglesFast.4.txt" i 0 using (2):3 with boxplot lc rgb COLOR_V, \
     "../vk.e.createTrianglesFast.8.txt" i 0 using (3):3 with boxplot lc rgb COLOR_V,

unset border
set label 2 "OpenGL" at graph 0.4, graph 0.9 font ",20" center
set title "Naïve"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH*4)
set rmargin at screen (RMARGIN)
set xrange[-2:2]
set xtics("1 Thread" 0)
plot "../ogl.b.createTrianglesHost.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_O, \



# unset arrow
# unset border
# unset title
# unset ylabel
# unset label 3
# set ytics format ""
# set label 2 "OpenGL" at graph 0.5, graph 0.9 font ",20" center
# set xrange[1:4]
# set xtics("Window" 2, "Total" 3)
# set rmargin at screen 0.9
# set lmargin at screen XSPLIT
# plot for[COL=1:2] "../ogl.a.initialize.1.40.2.txt" i 1 using (COL+1):COL with boxes lc rgb "#014477"
