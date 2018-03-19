
all: initializeCompare.png triangleCompare.png shaderCompare.png drawCompareTriangles.png drawComparePipelines.png stateChangeCompare.png

%.png: %.gp Header.gp $(shell find ../ -maxdepth 1 -type f )
	@gnuplot -c ./$<

