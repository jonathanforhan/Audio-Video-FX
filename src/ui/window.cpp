#include "window.hpp"

namespace avfx {

Window::Window(QWidget* parent)
  : QMainWindow(parent) {

    create_actions();
    create_menus();

    this->setGeometry(0, 0, 960, 540);
    this->setWindowState(Qt::WindowMaximized);
}

void Window::create_menus() {
    m_file_menu = menuBar()->addMenu(tr("File"));
    m_file_menu->addAction(m_new_act);
    m_file_menu->addAction(m_open_act);
    m_file_menu->addAction(m_import_act);
    m_file_menu->addAction(m_export_act);

    m_edit_menu = menuBar()->addMenu(tr("Edit"));
}

void Window::create_actions() {
    m_new_act    = new QAction(tr("New"), this);
    m_open_act   = new QAction(tr("Open"), this);
    m_import_act = new QAction(tr("Import"), this);
    m_export_act = new QAction(tr("Export"), this);
}

} // namespace avfx
