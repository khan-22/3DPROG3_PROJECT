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

  //TODO: NEED TO TIME THESE FOLLOWING FUNCTIONS
  createShaderModules(resultCollection);
  createPipelines(resultCollection);
}

void BenchOpenGL::createShaderModules(ResultCollection& resultCollection) {
  //TODO: Add timers
  //Timer t;
  //t.start("Loading");
  auto sourcePairOriginal = loadShaderSource();
  //t.stop();
  //resultCollection.addResult(t);

  for (auto &shaderPair : shaderPairArr)
  {
	  auto shaderPairCopy  = sourcePairOriginal;
	  auto define		   = getNextDefine();
	  shaderPairCopy.first = define + shaderPairCopy.first; //Add random defines (colors) to the top of the VS
	  shaderPair = shaderPairCopy;
	  //std::pair<gl::GLchar* const*, gl::GLchar* const*> test;
	  //test.first = shaderPairCopy.first;
	  //test.second = shaderPairCopy.second;
  }

}

void BenchOpenGL::createPipelines(ResultCollection& resultCollection) {

	for (int i = 0; i < shaderProgramArr.size(); ++i)
	{
		const char *src;
		GLuint VS, FS;
		VS = gl::glCreateShader(gl::GLenum::GL_VERTEX_SHADER);
		src = shaderPairArr[i].first.c_str();
		gl::glShaderSource(VS, 1, &src, NULL);
		gl::glCompileShader(VS);

		//Temporary
		int  success;
		gl::glGetShaderiv(VS, gl::GLenum::GL_COMPILE_STATUS, &success);

		if (!success)
		{
			printf("\n\nERROR COMPILING VS SHADER\n\n");
		}
		//

		FS = gl::glCreateShader(gl::GLenum::GL_FRAGMENT_SHADER);
		src = shaderPairArr[i].second.c_str();
		gl::glShaderSource(FS, 1, &src, NULL);
		gl::glCompileShader(FS);
	}

}

void BenchOpenGL::firstDraw(ResultCollection& resultCollection) {
}

void BenchOpenGL::secondDraw(ResultCollection& resultCollection) {
}

void BenchOpenGL::thirdDraw(ResultCollection& resultCollection) {
}

void BenchOpenGL::clean_up(ResultCollection& resultCollection) {
  glfwDestroyWindow(_window);
  glfwTerminate();  //
}
