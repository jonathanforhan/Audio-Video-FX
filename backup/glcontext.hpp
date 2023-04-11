#ifndef AVFX_SRC_GLCONTEXT_HPP
#define AVFX_SRC_GLCONTEXT_HPP

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtOpenGLWidgets>

#include <video.hpp>

namespace avfx {

class GLContext : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    GLContext();
    ~GLContext();

    int import_video(const char *path);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private:
    GLuint m_program;
    GLuint m_texture;
    GLuint m_vao, m_vbo, m_ebo;

    std::unique_ptr<Video> m_video;
};

} // namespace avfx

#endif // AVFX_SRC_GLCONTEXT_HPP
