#include "bench_opengl.hpp"

#include <iostream>

#include <GLFW/glfw3.h>
//
BenchOpenGL::BenchOpenGL(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  std::cout << "Started OpenGL Benchmark... (" << numberOfThreads << ", " << N
            << ", " << M << ")" << std::endl;
}

BenchOpenGL::~BenchOpenGL() {
  //
  std::cout << "Finished OpenGL Benchmark" << std::endl;
}

void BenchOpenGL::initialize(ResultCollection& resultCollection) {
  srand(0);

  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  Timer t;
  t.start("Window");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_WIDTH, _HEIGHT, _TITLE.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(_window);
  t.stop();
  resultCollection.addResult(t);

  glbinding::Binding::initialize();


  createShaderModules(resultCollection);
  createPipelines(resultCollection);
}

void BenchOpenGL::createTrianglesHost(ResultCollection& resultCollection) {
  //
}

void BenchOpenGL::createTrianglesSlow(ResultCollection& resultCollection) {
  //
}

void BenchOpenGL::createTrianglesSmart(ResultCollection& resultCollection) {
  //
}

void BenchOpenGL::createTrianglesFast(ResultCollection& resultCollection) {
  //
}

void BenchOpenGL::intermediateTriangleCleanUp() {
  //
}

void BenchOpenGL::createShaderModules(ResultCollection& resultCollection) {
  auto sourcePairOriginal = loadShaderSource();
 

  for (auto &shaderPair : shaderPairArr)
  {
	  auto shaderPairCopy  = sourcePairOriginal;
	  auto define		   = getNextDefine();
	  shaderPairCopy.first = define + shaderPairCopy.first; //Add random defines (colors) to the top of the VS
	  shaderPair		   = shaderPairCopy;
  }

}

void BenchOpenGL::createPipelines(ResultCollection& resultCollection) {

	for (int i = 0; i < shaderProgramArr.size(); ++i)
	{
		const char *src;
		GLuint VS, FS;
		VS	= gl::glCreateShader(gl::GLenum::GL_VERTEX_SHADER);
		src = shaderPairArr[i].first.c_str();
		gl::glShaderSource(VS, 1, &src, NULL);
		gl::glCompileShader(VS);

		FS	= gl::glCreateShader(gl::GLenum::GL_FRAGMENT_SHADER);
		src = shaderPairArr[i].second.c_str();
		gl::glShaderSource(FS, 1, &src, NULL);
		gl::glCompileShader(FS);

		GLuint shaderProgram;
		shaderProgram = gl::glCreateProgram();
		gl::glAttachShader(shaderProgram, VS);
		gl::glAttachShader(shaderProgram, FS);
		gl::glLinkProgram(shaderProgram);

		//Temporary
		int  success;
		gl::glGetShaderiv(shaderProgram, gl::GLenum::GL_LINK_STATUS, &success);
		if (!success)
		{
			printf("\n\nERROR CREATING SHADER PROGRAM\n\n");
		}
		//

		gl::glDeleteShader(VS);
		gl::glDeleteShader(FS);
	}

}

void BenchOpenGL::singleTriangleDraw(
	ResultCollection& resultCollection, bool device) {

	for (int i = 0; i < shaderProgramArr.size(); ++i) {
		gl::glUseProgram(shaderProgramArr.at(i));
		for (int j = 0; j < static_cast<int>(3/3); ++j) {
			gl::glDrawArrays(/**/, 0, 3);
		}
	}
}

void BenchOpenGL::optimalMultipleTriangleDraw(
    ResultCollection& resultCollection, bool device) {
  //
}

void BenchOpenGL::badMultipleTriangleDraw(ResultCollection& resultCollection,
                                          bool              device) {
  //
}

void BenchOpenGL::clean_up(ResultCollection& resultCollection) {
  glfwDestroyWindow(_window);
  glfwTerminate();  //
}
