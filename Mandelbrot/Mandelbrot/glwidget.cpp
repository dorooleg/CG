#define GLEW_STATIC
#include <GL/glew.h>
#include <glwidget.h>
#include <iostream>
#include <QWheelEvent>
#include "window.h"
#include <QWidget>
#include <QSlider>
#include <fstream>

const char* vertex_shader =
"#version 410\n"
"in vec3 vp;"
"void main () {"
"  gl_Position = vec4 (vp, 1.0);"
"}";

void compile_shader(GLuint &prog)
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    std::ifstream t("C:\\Users\\Oleg\\Downloads\\MyOpenGL\\MyOpenGL\\shader.glsl");
    if (!t.is_open()) {
        std::cerr << "Cannot open shader.glsl!" << std::endl;
        return;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    const char *src = str.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &src, NULL);
    glCompileShader(fs);

    int success;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        int s;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetShaderInfoLog(fs, s, &s, buf);

        std::cerr << buf << std::endl;
        delete[] buf;
        return;
    }

    prog = glCreateProgram();
    glAttachShader(prog, fs);
    glAttachShader(prog, vs);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        int s;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetProgramInfoLog(prog, s, &s, buf);

        std::cerr << buf << std::endl;
        delete[] buf;
        return;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , zoom(0.3)
    , iteration(100)
    , cx(0.0)
    , cy(0.0)
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void compile_shader(GLuint &prog);

void GLWidget::initializeGL()
{
    glewExperimental = GL_TRUE;
    glewInit();

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    compile_shader(prog);

    float points[] = {
        -1.0f,  1.0f,  0.0f,
        -1.0f,  -1.0f,  0.0f,
        1.0f,  -1.0f,  0.0f,

        -1.0f,  1.0f,  0.0f,
        1.0f,  -1.0f,  0.0f,
        1.0f,  1.0f,  0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUseProgram(prog);
    glBindVertexArray(vao);
}

void GLWidget::paintGL()
{
    glUniform2d(glGetUniformLocation(prog, "screen_size"), (double)width(), (double)height());
    glUniform1d(glGetUniformLocation(prog, "screen_ratio"), (double)width() / (double)height());
    glUniform2d(glGetUniformLocation(prog, "center"), cx, cy);
    glUniform1d(glGetUniformLocation(prog, "zoom"), zoom);
    glUniform1i(glGetUniformLocation(prog, "itr"), iteration);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    double oldZoom = zoom;
    std::cout << event->x() << " " << event->y() << std::endl;
    zoom += event->delta() * 0.01 * zoom;
    zoom = qMax(0.1, zoom);

    double oldX = -2.0 * (event->x() / (double)width() - 0.5) / oldZoom / 2.0;
    double oldY = 2.0 * (event->y() / (double)height() - 0.5) / oldZoom / 2.0;

    double newX = -2.0 * (event->x() / (double)width() - 0.5) / zoom / 2.0;
    double newY = 2.0 * (event->y() / (double)height() - 0.5) / zoom / 2.0;
    cx += newX - oldX;
    cy += newY - oldY;
    update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    prevPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    double dx = -2.0 * ((event->x() - prevPos.x()) / (double)width()) / zoom / 2.0;
    double dy = 2.0 * ((event->y() - prevPos.y()) / (double)height()) / zoom / 2.0;
    std::cout << event->x() - prevPos.x() << " " << dx << " " << dy << std::endl;
    cx += dx;
    cy += dy;
    prevPos = event->pos();
    update();
}

void GLWidget::setIteration(int iter)
{
    iteration = qMax(500 - iter, 10);
    update();
}
