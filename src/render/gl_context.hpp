#ifndef AVFX_SRC_RENDER_GL_CONTEXT_HPP
#define AVFX_SRC_RENDER_GL_CONTEXT_HPP

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>

#include "media/video.hpp"

namespace avfx {

class GLContext : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit GLContext(QWidget* parent = nullptr);
    virtual ~GLContext();

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

    size_t m_tex_i; // texture incriment

    Video m_video;
    QTimer* m_timer;
};

} // namespace avfx

#endif // AVFX_SRC_RENDER_GL_CONTEXT_HPP
