#include "opengl_widget.hpp"

namespace avfx {

OpenGLWidget::OpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent),
    m_vao(QOpenGLVertexArrayObject()),
    m_vbo(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
    m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)),
    m_textures(std::vector<QOpenGLTexture*>()),
    m_num_textures(0), m_video(Video::import("samples/example.mp4")) {}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();
    m_program.destroyed();
    for (size_t i = 0; i < m_video->frames().size(); ++i) {
        m_textures[i]->destroy();
        delete m_textures[i];
    }

    doneCurrent();
}

void OpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

    m_program.create();
    m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
            "shaders/basic.vert");
    m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
            "shaders/basic.frag");
    m_program.link();

    m_vao.create();
    m_vao.bind();

    float vertices[20] = {
        // positions          // texture coords
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top right
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(20 * sizeof(float));
    m_vbo.write(0, vertices, 20 * sizeof(float));

    GLsizei stride = 5 * sizeof(float);
    auto tex_ptr   = (GLvoid*)(3 * sizeof(float));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, tex_ptr);
    glEnableVertexAttribArray(1);

    uint32_t indices[6] = { 0, 1, 3, 1, 2, 3 };

    m_ebo.create();
    m_ebo.bind();
    m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ebo.allocate(6 * sizeof(uint32_t));
    m_ebo.write(0, indices, 6 * sizeof(uint32_t));

    m_vbo.release(QOpenGLBuffer::VertexBuffer);
    m_vao.release();

    size_t n = m_video->frames().size();
    m_textures.resize(n);

    typedef QOpenGLTexture GLTex;
    for (size_t i = 0; i < n; ++i) {
        m_textures[i] = new GLTex(GLTex::Target2D);
        m_textures[i]->create();
        m_textures[i]->bind(GLTex::Target2D);
        m_textures[i]->setWrapMode(GLTex::Repeat);
        m_textures[i]->setMinificationFilter(GLTex::LinearMipMapLinear);
        m_textures[i]->setMagnificationFilter(GLTex::Linear);

        int w = m_video->frame(i)->width;
        int h = m_video->frame(i)->height;
        QImage img(m_video->frame(i)->data, w, h, QImage::Format_RGB888);

        m_textures[i]->setData(img);
    }
}

void OpenGLWidget::resizeGL(int w, int h) {
    // Set the viewport to the entire widget area
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL() {
    m_program.bind();
    m_vao.bind();
    m_textures[m_num_textures++]->bind();
    if (m_num_textures == m_video->frames().size()) {
        m_num_textures = 0;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_vao.release();
    m_program.release();

    update();
}

} // namespace avfx
