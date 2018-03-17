call "Header.gp" 1028 (768 - 200)

set output "initializeCompare.png"

set multiplot
set xtics nomirror

set key autotitle columnheader
set key off
set yrange[0:0.20]
set grid y
set xtics rotate by -45
set style fill solid 0.7
set boxwidth 0.7
# set facecolor #FFFFFF

set tmargin at screen 0.8
set bmargin at screen 0.2

# set key outside

set label 2 "Vulkan" at graph 0.1, graph 0.9 font ",20" center

unset border
set title "Average time spent initializing" font ",18"

# stats "../vk.a.initialize.2.40.2.txt" using 6
set ylabel "Time in seconds"

set label 3 "Significant driver involvement" at 3, screen 0.64 font ",16"
set arrow 1 from 3, screen 0.6 to 3, screen 0.5 head size 5.0,2.0 front filled
set arrow 2 from 6, screen 0.6 to 6, screen 0.5 head size 5.0,2.0 front filled
set arrow 3 from 7, screen 0.6 to 7, screen 0.5 head size 5.0,2.0 front filled

set border 8
XSPLIT=0.75
set rmargin at screen XSPLIT
# set xlabel "Stage"
set xrange[1:15]
set xtics("Window" 2,	"Instance" 3,	"Surface" 4,	"PhysicalDevice" 5,	"Device" 6,	"Swapchain" 7,	"RenderPass" 8,	"Framebuffers" 9,	"CommandPool" 10,	"CommandBuffers" 11,	"PipelineLayout" 12,	"Semaphores"  13,	"Total" 14)
plot for[COL=1:13] "../vk.a.initialize.1.txt" i 1 using (COL+1):COL with boxes lc rgb COLOR_V

unset arrow
unset border
unset title
unset ylabel
unset label 3
set ytics format ""
set label 2 "OpenGL" at graph 0.5, graph 0.9 font ",20" center
set xrange[1:4]
set xtics("Window" 2, "Total" 3)
set rmargin at screen 0.9
set lmargin at screen XSPLIT
plot for[COL=1:2] "../ogl.a.initialize.1.txt" i 1 using (COL+1):COL with boxes lc rgb COLOR_O
