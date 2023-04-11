#include "glcontext.hpp"

namespace avfx {

const char *vert_str =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main() {\n"
    "     gl_Position = vec4(aPos, 1.0);\n"
    "     texCoord = aTexCoord;\n"
    "}\0";
const char *frag_str =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D tex;\n"
    "void main() {\n"
    "     vec2 texCoord = vec2(texCoord.x, -texCoord.y);\n"
    "     FragColor = texture(tex, texCoord);\n"
    "}\0";

const float vertices[] = {
    // position        // texture coords
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top right
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
   -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
};
const uint32_t indices[] = {
    0, 1, 3, // first Triangle
    1, 2, 3  // second Triangle
};

GLContext::GLContext() :
    m_video(nullptr)
{}

GLContext::~GLContext() {
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    f->glDeleteVertexArrays(1, &m_vao);
    f->glDeleteBuffers(1, &m_vbo);
    f->glDeleteBuffers(1, &m_ebo);
    f->glDeleteProgram(m_program);
}

int GLContext::import_video(const char *p_path) {
    m_video = Video::import(p_path);
    return m_video == nullptr ? -1 : 0;
}

void GLContext::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

void GLContext::resizeGL(int p_width, int p_height) {
}

void GLContext::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(  1.0,  0.0, 0.0);
        glVertex3f(-0.5, -0.5, 0);
        glColor3f(  0.0,  1.0, 0.0);
        glVertex3f( 0.5, -0.5, 0);
        glColor3f(  0.0,  0.0, 1.0);
        glVertex3f( 0.0,  0.5, 0);
    glEnd();
}

} // namespace avfx

