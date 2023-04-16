#include <QApplication>
#include <QSurfaceFormat>

#include "media/video.hpp"
#include "render/gl_context.hpp"
#include "ui/window.hpp"

using namespace avfx;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create a surface format with OpenGL 3.3 core profile
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    Window window;
    window.show();

    return app.exec();
}
