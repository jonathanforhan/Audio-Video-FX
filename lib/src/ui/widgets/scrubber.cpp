#include "scrubber.hpp"

namespace avfx {

Scrubber::Scrubber(const QString& title, QWidget* parent)
  : QGroupBox(title, parent) {
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setFocusPolicy(Qt::StrongFocus);
    m_slider->setTickPosition(QSlider::TicksAbove);
    m_slider->setTickInterval(5);
    m_slider->setSingleStep(1);
}

void Scrubber::setSize(int x, int y, int w, int h) {
    m_slider->setGeometry(x, y, w, h);
}

} // namespace avfx
