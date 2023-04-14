#include "opengl_widget.hpp"
#include <iostream>

namespace avfx {

OpenGLWidget::OpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent),
    m_vao(QOpenGLVertexArrayObject()),
    m_vbo(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
    m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)),
    m_textures(std::vector<QOpenGLTexture*>()),
    m_tex_i(0),
    m_video("samples/example.mp4"),
    m_timer(new QTimer(this)) {

    m_video.stream_decode();
}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();
    m_ebo.destroy();
    m_program.destroyed();
    for (auto& texture : m_textures) {
        texture->destroy();
        delete texture;
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

    m_video.lock_stream();
    while (m_video.frame_count() < 1)
        ;
    resize(m_video.frame(0)->width, m_video.frame(0)->height);

    connect(m_timer, &QTimer::timeout, this,
            (void(QWidget::*)()) & QOpenGLWidget::update);
    m_timer->start(1000 / m_video.frame_rate());

    m_video.unlock_stream();
}

void OpenGLWidget::resizeGL(int w, int h) {
    // Set the viewport to the entire widget area
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL() {
    typedef QOpenGLTexture GLTex;

    m_video.lock_stream();
    m_video.log_decode();
    if (m_tex_i < m_video.frame_count() && m_textures.size() < m_video.frame_count()) {
        m_textures.emplace_back(new GLTex(GLTex::Target2D));
        m_textures[m_tex_i]->create();
        m_textures[m_tex_i]->bind(GLTex::Target2D);
        m_textures[m_tex_i]->setWrapMode(GLTex::Repeat);
        m_textures[m_tex_i]->setMinificationFilter(GLTex::LinearMipMapLinear);
        m_textures[m_tex_i]->setMagnificationFilter(GLTex::Linear);

        int w = m_video.frame(m_tex_i)->width;
        int h = m_video.frame(m_tex_i)->height;
        QImage img(m_video.frame(m_tex_i)->data, w, h, QImage::Format_RGB888);

        m_textures[m_tex_i++]->setData(img);

    } else if (m_video.stream_done()) {
        if (m_tex_i == m_video.frame_count()) {
            m_tex_i = 0;
        }
        m_textures[m_tex_i++]->bind();
    }
    // printf("frame num : %lu\n", m_video.frame_count());
    m_video.unlock_stream();


    m_program.bind();
    m_vao.bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_vao.release();
    m_program.release();

    // update();
}

} // namespace avfx
