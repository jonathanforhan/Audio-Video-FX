#ifndef AVFX_SRC_CORE_HELLO_WINDOW_HPP
#define AVFX_SRC_CORE_HELLO_WINDOW_HPP


#include <QWindow>

#include <QColor>
#include <QMutex>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QSharedPointer>
#include <QTimer>

namespace avfx {

class HelloWindow;

class Renderer : public QObject
{
    Q_OBJECT
public:
    explicit Renderer(const QSurfaceFormat &format, Renderer *share = 0, QScreen *screen = 0);
    virtual ~Renderer() {};

    QSurfaceFormat format() const { return m_format; }

    void setAnimating(HelloWindow *window, bool animating);

public slots:
    void render();

private:
    void initialize();

    void createGeometry();
    void createBubbles(int number);
    void quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4);
    void extrude(qreal x1, qreal y1, qreal x2, qreal y2);

    qreal m_fAngle;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    int vertexAttr;
    int normalAttr;
    int matrixUniform;
    int colorUniform;

    bool m_initialized;
    QSurfaceFormat m_format;
    QOpenGLContext *m_context;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer m_vbo;

    QList<HelloWindow *> m_windows;
    int m_currentWindow;

    QMutex m_windowLock;

    QColor m_backgroundColor;
};

class HelloWindow : public QWindow
{
public:
    explicit HelloWindow(const QSharedPointer<Renderer> &renderer, QScreen *screen = 0);

    QColor color() const;
    void updateColor();

protected:
    bool event(QEvent *ev) override;
    void exposeEvent(QExposeEvent *event) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    int m_colorIndex;
    QColor m_color;
    const QSharedPointer<Renderer> m_renderer;
    mutable QMutex m_colorLock;
};

} // namespace avfx

#endif // AVFX_SRC_CORE_HELLO_WINDOW_HPP
