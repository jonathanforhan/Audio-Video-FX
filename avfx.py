import sys
from PyQt6.QtWidgets import \
    QApplication,           \
    QLabel,                 \
    QMainWindow,            \
    QPushButton,            \
    QHBoxLayout

from src import GLWidget


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Audio-Video FX")
        self.resize(1000, 800)
        self.centralWidget = GLWidget(self)
        self.setCentralWidget(self.centralWidget)

    def _createMenuBar(self):
        ...


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
