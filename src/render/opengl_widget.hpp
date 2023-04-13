#ifndef AVFX_SRC_RENDER_OPENGL_WIDGET_HPP
#define AVFX_SRC_RENDER_OPENGL_WIDGET_HPP

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

namespace avfx {

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    OpenGLWidget(QWidget *parent=nullptr);
    ~OpenGLWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;
    QOpenGLShaderProgram m_program;
};

} // namespace avfx

#endif // AVFX_SRC_RENDER_OPENGL_WIDGET_HPP
