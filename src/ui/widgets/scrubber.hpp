#ifndef AVFX_SRC_UI_WIDGETS_SCRUBBER_HPP
#define AVFX_SRC_UI_WIDGETS_SCRUBBER_HPP

#include <QBoxLayout>
#include <QDial>
#include <QGroupBox>
#include <QScrollBar>
#include <QSlider>

namespace avfx {

class Scrubber : public QGroupBox {
    Q_OBJECT

public:
    Scrubber(const QString& title, QWidget* parent = nullptr);

    void setSize(int x, int y, int w, int h);

private:
    QSlider* m_slider;
};

} // namespace avfx

#endif // AVFX_SRC_UI_WIDGETS_SCRUBBER_HPP
