call "Header.gp" 900 (768 - 200)

set output "drawComparePipelines.png"

set multiplot
set xtics nomirror

set key autotitle columnheader
set key off
set grid y
set xtics rotate by -45
set style fill solid 0.7
set boxwidth 0.7
# set facecolor #FFFFFF

set tmargin at screen 0.80
set bmargin at screen 0.2

# set key outside


unset border
# set title "Average time spent initializing" font ",18"

# stats "../vk.a.initialize.2.40.2.txt" using 6

stats "../ogl.k.firstDrawPipelines.1.txt" i 0 using 2 nooutput

# set label 3 "Significant driver involvement" at 3, screen 0.64 font ",16"
# set arrow 1 from 3, screen 0.6 to 3, screen 0.5 head size 5.0,2.0 front filled
# set arrow 2 from 6, screen 0.6 to 6, screen 0.5 head size 5.0,2.0 front filled
# set arrow 3 from 7, screen 0.6 to 7, screen 0.5 head size 5.0,2.0 front filled

set border 8
LMARGIN=0.10
XWIDTH=0.17
XWIDTH2=0.11
RMARGIN=0.94

set style rectangle

set label 3 "Using N pipelines to render 1 triangle" at screen 0.5, screen 0.93 font ",20" front center

LABEL2="Vulkan"
set object 2 rect at graph 0.5, graph 0.87 size char strlen(LABEL2)+5, char 1 fc "white" front  fs noborder
set label 2 "Vulkan" at graph 0.5, graph 0.87 font ",20" center front
set title "First time"
set ylabel "Time in seconds" offset 2.5 font ",16"
set lmargin at screen (LMARGIN)
set rmargin at screen (LMARGIN+XWIDTH)
set xrange[-1:4]
set yrange[0:0.42]
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.k.firstDrawPipelines.1.txt" i 0 using (0):5 with boxplot lc rgb COLOR_V, \
     "../vk.k.firstDrawPipelines.2.txt" i 0 using (1):5 with boxplot lc rgb COLOR_V, \
     "../vk.k.firstDrawPipelines.4.txt" i 0 using (2):5 with boxplot lc rgb COLOR_V, \
     "../vk.k.firstDrawPipelines.8.txt" i 0 using (3):5 with boxplot lc rgb COLOR_V,

# set label 2 "Vulkan" at graph 0.3, graph 0.85 font ",20" center
unset object 2
unset label 2
set title "Second time"
unset ylabel
set lmargin at screen (LMARGIN+XWIDTH)
set rmargin at screen (LMARGIN+XWIDTH*2)
set ytics format ""
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.l.secondDrawPipelines.1.txt" i 0 using (0):5 with boxplot lc rgb COLOR_V, \
     "../vk.l.secondDrawPipelines.2.txt" i 0 using (1):5 with boxplot lc rgb COLOR_V, \
     "../vk.l.secondDrawPipelines.4.txt" i 0 using (2):5 with boxplot lc rgb COLOR_V, \
     "../vk.l.secondDrawPipelines.8.txt" i 0 using (3):5 with boxplot lc rgb COLOR_V,

unset label 2
set title "Third time"
unset ylabel
set lmargin at screen (LMARGIN+XWIDTH*2)
set rmargin at screen (LMARGIN+XWIDTH*3)
set ytics format ""
set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
plot "../vk.m.thirdDrawPipelines.1.txt" i 0 using (0):5 with boxplot lc rgb COLOR_V, \
     "../vk.m.thirdDrawPipelines.2.txt" i 0 using (1):5 with boxplot lc rgb COLOR_V, \
     "../vk.m.thirdDrawPipelines.4.txt" i 0 using (2):5 with boxplot lc rgb COLOR_V, \
     "../vk.m.thirdDrawPipelines.8.txt" i 0 using (3):5 with boxplot lc rgb COLOR_V,



# set label 2 "Vulkan" at graph 0.3, graph 0.85 font ",20" center
# set title "Spir-V into native\n+ Pipeline creation"
# unset ylabel
# set ytics format ""
# set lmargin at screen (LMARGIN+XWIDTH)
# set rmargin at screen (LMARGIN+XWIDTH*2)
# set xrange[-1:4]
# set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
# plot "../vk.g.createPipelines.1.txt" i 0 using (0):2 with boxplot lc rgb "#770111", \
#      "../vk.g.createPipelines.2.txt" i 0 using (1):2 with boxplot lc rgb "#770111", \
#      "../vk.g.createPipelines.4.txt" i 0 using (2):2 with boxplot lc rgb "#770111", \
#      "../vk.g.createPipelines.8.txt" i 0 using (3):2 with boxplot lc rgb "#770111",



LABEL2="OpenGL"
set object 2 rect at graph 0.5, graph 0.87 size char strlen(LABEL2)+5, char 1 fc "white" front  fs noborder
set label 2 "OpenGL" at graph 0.5, graph 0.87 font ",20" center front
# unset label 2
set title "First time"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH*3)
set rmargin at screen (LMARGIN+XWIDTH*3+XWIDTH2)
set xrange[-2:2]
set xtics("1 Thread" 0)
plot "../ogl.k.firstDrawPipelines.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_O, \



unset label 2
# set label 2 "OpenGL" at graph 0.4, graph 0.85 font ",20" center
set title "Second time"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH*3+XWIDTH2)
set rmargin at screen (LMARGIN+XWIDTH*3+XWIDTH2*2)
set xrange[-2:2]
set xtics("1 Thread" 0)
plot "../ogl.l.secondDrawPipelines.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_O, \

unset border
unset label 2
# set label 2 "OpenGL" at graph 0.4, graph 0.85 font ",20" center
set title "Third time"
unset ylabel
set ytics format ""
set lmargin at screen (LMARGIN+XWIDTH*3+XWIDTH2*2)
set rmargin at screen (RMARGIN)
set xrange[-2:2]
set xtics("1 Thread" 0)
plot "../ogl.m.thirdDrawPipelines.1.txt" i 0 using (0):2 with boxplot lc rgb COLOR_O, \





# unset label 2
# set title "\"Wasteful\"\n(Over-synchronization)"
# unset ylabel
# set ytics format ""
# set lmargin at screen (LMARGIN+XWIDTH)
# set rmargin at screen (LMARGIN+XWIDTH*2)
# set xrange[-1:4]
# set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
# plot "../vk.c.createTrianglesSlow.1.txt" i 0 using (0):2 with boxplot lc rgb "#770111", \
#      "../vk.c.createTrianglesSlow.2.txt" i 0 using (1):2 with boxplot lc rgb "#770111", \
#      "../vk.c.createTrianglesSlow.4.txt" i 0 using (2):2 with boxplot lc rgb "#770111", \
#      "../vk.c.createTrianglesSlow.8.txt" i 0 using (3):2 with boxplot lc rgb "#770111",

# set title "\"Smart\"\n(Command daisy-chain)"
# unset ylabel
# set ytics format ""
# set lmargin at screen (LMARGIN+XWIDTH*2)
# set rmargin at screen (LMARGIN+XWIDTH*3)
# set xrange[-1:4]
# set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
# plot "../vk.d.createTrianglesSmart.1.txt" i 0 using (0):2 with boxplot lc rgb "#770111", \
#      "../vk.d.createTrianglesSmart.2.txt" i 0 using (1):2 with boxplot lc rgb "#770111", \
#      "../vk.d.createTrianglesSmart.4.txt" i 0 using (2):2 with boxplot lc rgb "#770111", \
#      "../vk.d.createTrianglesSmart.8.txt" i 0 using (3):2 with boxplot lc rgb "#770111",

# set title "\"Fast\"\n(Deferred submission)"
# unset ylabel
# set ytics format ""
# set lmargin at screen (LMARGIN+XWIDTH*3)
# set rmargin at screen (LMARGIN+XWIDTH*4)
# set xrange[-1:4]
# set xtics("1 Thread" 0, "2 Threads" 1, "4 Threads" 2, "8 Threads" 3)
# plot "../vk.e.createTrianglesFast.1.txt" i 0 using (0):2 with boxplot lc rgb "#770111", \
#      "../vk.e.createTrianglesFast.2.txt" i 0 using (1):2 with boxplot lc rgb "#770111", \
#      "../vk.e.createTrianglesFast.4.txt" i 0 using (2):2 with boxplot lc rgb "#770111", \
#      "../vk.e.createTrianglesFast.8.txt" i 0 using (3):2 with boxplot lc rgb "#770111",

# unset border
# set label 2 "OpenGL" at graph 0.4, graph 0.9 font ",20" center
# set title "Naïve"
# unset ylabel
# set ytics format ""
# set lmargin at screen (LMARGIN+XWIDTH*4)
# set rmargin at screen (RMARGIN)
# set xrange[-2:2]
# set xtics("1 Thread" 0)
# plot "../ogl.b.createTrianglesHost.1.txt" i 0 using (0):2 with boxplot lc rgb "#014477", \
















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
