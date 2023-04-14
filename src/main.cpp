#include <QApplication>
#include <QSurfaceFormat>

#include "core/video.hpp"
#include "render/opengl_widget.hpp"

using namespace avfx;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create a surface format with OpenGL 3.3 core profile
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // Create a window with an OpenGL context
    OpenGLWidget widget;
    widget.show();

    return app.exec();
}
