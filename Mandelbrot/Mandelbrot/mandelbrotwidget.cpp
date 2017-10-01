#define GLEW_STATIC
#include <GL/glew.h>
#include <mandelbrotwidget.h>
#include <iostream>
#include <QWheelEvent>
#include "window.h"
#include <QWidget>
#include <QSlider>
#include <fstream>

void MandelbrotWidget::compileShader(const std::string& fragmentShader, const std::string& vertexShader, GLuint &prog)
{
    std::ifstream vertexFile(vertexShader);
    if (!vertexFile.is_open()) {
        std::cerr << "Cannot open " << vertexShader << std::endl;
        return;
    }

    std::ifstream fragmentFile(fragmentShader);
    if (!fragmentFile.is_open()) {
        std::cerr << "Cannot open " << fragmentShader << std::endl;
        return;
    }

    std::string vertexString((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
    const char *vertexData = vertexString.c_str();

    std::string fragmentString((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());
    const char *fragmentData = fragmentString.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexData, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentData, NULL);
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

MandelbrotWidget::MandelbrotWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , zoom(0.3)
    , cx(0.0)
    , cy(0.0)
    , iteration(100)
{
}

QSize MandelbrotWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MandelbrotWidget::sizeHint() const
{
    return QSize(400, 400);
}

void MandelbrotWidget::initializeGL()
{
    glewInit();

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    compileShader("../Mandelbrot/shaders/mandelbrot.frag", "../Mandelbrot/shaders/mandelbrot.vert", prog);

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

void MandelbrotWidget::paintGL()
{
    glUniform2d(glGetUniformLocation(prog, "screen_size"), (double)width(), (double)height());
    glUniform1d(glGetUniformLocation(prog, "screen_ratio"), (double)width() / (double)height());
    glUniform2d(glGetUniformLocation(prog, "center"), cx, cy);
    glUniform1d(glGetUniformLocation(prog, "zoom"), zoom);
    glUniform1i(glGetUniformLocation(prog, "itr"), iteration);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MandelbrotWidget::resizeGL(int width, int height)
{
    const int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void MandelbrotWidget::wheelEvent(QWheelEvent *event)
{
    const double oldZoom = zoom;
    zoom += event->delta() * 0.001 * zoom;
    zoom = qMax(0.1, zoom);

    const double oldX = -(event->x() / (double)width() - 0.5) / oldZoom;
    const double oldY = (event->y() / (double)height() - 0.5) / oldZoom;

    const double newX = -(event->x() / (double)width() - 0.5) / zoom;
    const double newY = (event->y() / (double)height() - 0.5) / zoom;
    cx += newX - oldX;
    cy += newY - oldY;

    update();
}

void MandelbrotWidget::mousePressEvent(QMouseEvent *event)
{
    prevPos = event->pos();
}

void MandelbrotWidget::mouseMoveEvent(QMouseEvent *event)
{
    const double dx = -(event->x() - prevPos.x()) / (double)width() / zoom;
    const double dy = (event->y() - prevPos.y()) / (double)height() / zoom;
    cx += dx;
    cy += dy;
    prevPos = event->pos();
    update();
}

void MandelbrotWidget::setIteration(int iter)
{
    iteration = qMax(500 - iter, 10);
    update();
}
