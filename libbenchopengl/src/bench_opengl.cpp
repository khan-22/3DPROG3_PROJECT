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
  if (!_window) {
    std::cerr << "Failed to create window" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(_window);
  t.stop();
  resultCollection.addResult(t);

  glbinding::Binding::initialize();
  gl::glClearColor(0.f, 0.3f, 0.6f, 1.f);

  gl::glFinish();
}

void BenchOpenGL::createTrianglesHost(ResultCollection& resultCollection) {
  for (int i = 0; i < BENCHMARK_N; ++i) {
    gl::glGenVertexArrays(1, &VAOArr[i]);
    gl::glBindVertexArray(VAOArr[i]);

    std::array<Vertex, 3> triangle = getNextTriangle();
    gl::glGenBuffers(1, &VBOArr[i]);
    gl::glBindBuffer(gl::GLenum::GL_ARRAY_BUFFER, VBOArr[i]);
    gl::glBufferData(gl::GLenum::GL_ARRAY_BUFFER,
                     triangle.size() * sizeof(triangle[0]),
                     triangle.data(),
                     gl::GLenum::GL_STATIC_DRAW);

    gl::glVertexAttribPointer(
        0, 3, gl::GLenum::GL_FLOAT, false, sizeof(Vertex), (void*)0);
    gl::glEnableVertexAttribArray(0);

    gl::glBindBuffer(gl::GLenum::GL_ARRAY_BUFFER, 0);
    gl::glBindVertexArray(0);
  }

  gl::glFinish();
}

void BenchOpenGL::createTrianglesSlow(ResultCollection& resultCollection) {
  createTrianglesHost(resultCollection);
}

void BenchOpenGL::createTrianglesSmart(ResultCollection& resultCollection) {
  createTrianglesHost(resultCollection);
}

void BenchOpenGL::createTrianglesFast(ResultCollection& resultCollection) {
  createTrianglesHost(resultCollection);
}

void BenchOpenGL::intermediateTriangleCleanUp() {
  for (auto VAO : VAOArr) {
    gl::glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }
  for (auto VBO : VBOArr) {
    gl::glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
}

void BenchOpenGL::createShaderModules(ResultCollection& resultCollection) {
  auto sourcePairOriginal = loadShaderSource();

  for (auto& shaderPair : shaderPairArr) {
    auto shaderPairCopy = sourcePairOriginal;
    auto define         = getNextDefine();
    shaderPairCopy.first =
        define +
        shaderPairCopy
            .first;  // Add random defines (colors) to the top of the VS
    shaderPair = shaderPairCopy;
  }

  gl::glFinish();
}

void BenchOpenGL::createPipelines(ResultCollection& resultCollection) {
  for (int i = 0; i < shaderProgramArr.size(); ++i) {
    const char* src;
    gl::GLuint  VS, FS;
    VS  = gl::glCreateShader(gl::GLenum::GL_VERTEX_SHADER);
    src = shaderPairArr[i].first.c_str();
    gl::glShaderSource(VS, 1, &src, NULL);
    gl::glCompileShader(VS);

    FS  = gl::glCreateShader(gl::GLenum::GL_FRAGMENT_SHADER);
    src = shaderPairArr[i].second.c_str();
    gl::glShaderSource(FS, 1, &src, NULL);
    gl::glCompileShader(FS);

    gl::GLuint shaderProgram;
    shaderProgram = gl::glCreateProgram();
    gl::glAttachShader(shaderProgram, VS);
    gl::glAttachShader(shaderProgram, FS);
    gl::glLinkProgram(shaderProgram);

    // Temporary
    int  success;
    char infoLog[1024];

    gl::glGetShaderiv(VS, gl::GLenum::GL_COMPILE_STATUS, &success);
    if (success == false) {
      gl::glGetShaderInfoLog(VS, 1024, nullptr, infoLog);
      std::cout << "ERROR VERTEX Shader linking failed!\n"
                << infoLog << std::endl;
    }

    gl::glGetShaderiv(FS, gl::GLenum::GL_COMPILE_STATUS, &success);
    if (success == false) {
      gl::glGetShaderInfoLog(FS, 1024, nullptr, infoLog);
      std::cout << "ERROR FRAGMENT Shader linking failed!\n"
                << infoLog << std::endl;
    }

    gl::glGetProgramiv(shaderProgram, gl::GLenum::GL_LINK_STATUS, &success);
    if (success == false) {
      gl::glGetShaderInfoLog(shaderProgram, 1024, nullptr, infoLog);
      std::cout << "ERROR Shader linking failed!\n" << infoLog << std::endl;
    }
    //

    shaderProgramArr[i] = shaderProgram;

    gl::glDetachShader(shaderProgram, VS);
    gl::glDetachShader(shaderProgram, FS);

    gl::glDeleteShader(VS);
    gl::glDeleteShader(FS);
  }

  gl::glFinish();
}

void BenchOpenGL::singleTriangleDraw(ResultCollection& resultCollection,
                                     bool              device) {
  gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < shaderProgramArr.size(); ++i) {
    gl::glUseProgram(shaderProgramArr.at(i));
    gl::glBindVertexArray(VAOArr.at(i));
    gl::glDrawArrays(gl::GLenum::GL_TRIANGLES, 0, 3);
  }

  glfwSwapBuffers(_window);
  gl::glFinish();
}

void BenchOpenGL::optimalMultipleTriangleDraw(
    ResultCollection& resultCollection, bool device) {
  gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);
  for (int i = 0; i < shaderProgramArr.size(); ++i) {
    gl::glUseProgram(shaderProgramArr.at(i));
    for (int j = 0; j < BENCHMARK_M; ++j) {
      gl::glBindVertexArray(VAOArr.at(j));
      gl::glDrawArrays(gl::GLenum::GL_TRIANGLES, 0, 3);
    }
  }

  glfwSwapBuffers(_window);

  gl::glFinish();
}

void BenchOpenGL::badMultipleTriangleDraw(ResultCollection& resultCollection,
                                          bool              device) {
  gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT);
  for (int i = 0; i < BENCHMARK_M; ++i) {
    for (int j = 0; j < shaderProgramArr.size(); ++j) {
      gl::glUseProgram(shaderProgramArr.at(j));
      gl::glBindVertexArray(VAOArr.at(j));
      gl::glDrawArrays(gl::GLenum::GL_TRIANGLES, 0, 3);
    }
  }

  glfwSwapBuffers(_window);

  gl::glFinish();
}

void BenchOpenGL::clean_up(ResultCollection& resultCollection) {
  glfwDestroyWindow(_window);
  glfwTerminate();  //
}
