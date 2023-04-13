#include <QApplication>
#include <QSurfaceFormat>

#include "render/opengl_widget.hpp"
#include "core/video.hpp"

#include <iostream>
using namespace avfx;

int main(int argc, char *argv[]) {
    // QApplication app(argc, argv);
    //
    // // Create a surface format with OpenGL 3.3 core profile
    // QSurfaceFormat format;
    // format.setVersion(3, 3);
    // format.setProfile(QSurfaceFormat::CoreProfile);
    // QSurfaceFormat::setDefaultFormat(format);
    //
    // // Create a window with an OpenGL context
    // OpenGLWidget widget;
    // widget.show();
    //
    // return app.exec();

    auto video = Video::import("samples/example.mp4");
    int w = video->get_frame_width(0);
    int h = video->get_frame_height(0);

    std::cout << w << " " << h << '\n';
}
