#include "gl_context.hpp"

#include <bit>

namespace avfx {

GLContext::GLContext(QWidget* parent)
  : QOpenGLWidget(parent),
    m_vao(QOpenGLVertexArrayObject()),
    m_vbo(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
    m_ebo(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)),
    m_textures(std::vector<QOpenGLTexture*>()),
    m_tex_i(0),
    m_video(),
    m_timer(new QTimer(this)) {}

GLContext::~GLContext() {
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

void GLContext::initializeGL() {
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

    constexpr float vertices[20] = {
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

    constexpr GLsizei stride  = 5 * sizeof(float);
    const void* const tex_ptr = (const void*)(3 * sizeof(float));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, tex_ptr);
    glEnableVertexAttribArray(1);

    constexpr uint32_t indices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    m_ebo.create();
    m_ebo.bind();
    m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ebo.allocate(6 * sizeof(uint32_t));
    m_ebo.write(0, indices, 6 * sizeof(uint32_t));

    m_vbo.release(QOpenGLBuffer::VertexBuffer);
    m_vao.release();
}

void GLContext::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLContext::paintGL() {
    m_program.bind();
    m_vao.bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_vao.release();
    m_program.release();

    update();
}

} // namespace avfx
