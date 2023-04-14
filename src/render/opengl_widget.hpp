#ifndef AVFX_SRC_RENDER_OPENGL_WIDGET_HPP
#define AVFX_SRC_RENDER_OPENGL_WIDGET_HPP

#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include "core/video.hpp"

namespace avfx {

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    OpenGLWidget(QWidget* parent = nullptr);
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
    std::vector<QOpenGLTexture*> m_textures;
    size_t m_num_textures;

    std::unique_ptr<Video> m_video;
};

} // namespace avfx

#endif // AVFX_SRC_RENDER_OPENGL_WIDGET_HPP
