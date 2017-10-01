#pragma once

#include <QGLWidget>
#include <string>

class MandelbrotWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MandelbrotWidget(QWidget *parent = 0);
    virtual ~MandelbrotWidget() = default;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:
    void setIteration(int iter);

private:
    static void compileShader(const std::string& fragmentShader, const std::string& vertexShader, GLuint &prog);

private:
    GLuint prog;
    double zoom;
    double cx;
    double cy;
    int iteration;
    QPoint prevPos;
};

