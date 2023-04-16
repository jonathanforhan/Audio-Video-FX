#ifndef AVFX_SRC_UI_WINDOW_HPP
#define AVFX_SRC_UI_WINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QWidget>

#include "render/gl_context.hpp"
#include "ui/widgets/scrubber.hpp"

namespace avfx {

class Window : public QMainWindow {
    Q_OBJECT

public:
    Window(QWidget* parent = nullptr);

private:
    void resizeEvent(QResizeEvent*) override{};

    /* Menus */
    void create_menus();
    QMenu* m_file_menu;
    QMenu* m_edit_menu;

    /* Actions within menus */
    void create_actions();
    QAction* m_new_act;
    QAction* m_open_act;
    QAction* m_import_act;
    QAction* m_export_act;
};

} // namespace avfx

#endif // AVFX_SRC_UI_WINDOW_HPP
