
all: initializeCompare.png triangleCompare.png shaderCompare.png drawCompare.png

%.png: %.gp Header.gp $(shell find ../ -maxdepth 1 -type f )
	@gnuplot -c ./$<

