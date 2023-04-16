import ctypes
import numpy as np
from OpenGL import GL as gl
from PyQt6.QtOpenGLWidgets import QOpenGLWidget


class GLWidget(QOpenGLWidget):
    def __init__(self, parent):
        super(GLWidget, self).__init__(parent)
        self.setMinimumSize(640, 480)

    def __del__(self):
        gl.glDeleteVertexArrays(1, np.array(self.vao))
        gl.glDeleteBuffers(1, np.array(self.vbo))
        gl.glDeleteProgram(self.program)

    def initializeGL(self):
        v_str, f_str = "", ""
        with open("./shaders/basic.vert") as f:
            v_str = f.read()
        with open("./shaders/basic.frag") as f:
            f_str = f.read()

        # Vertex Shader
        self.vertex_shader = gl.glCreateShader(gl.GL_VERTEX_SHADER)
        gl.glShaderSource(self.vertex_shader, v_str)
        gl.glCompileShader(self.vertex_shader)

        # Fragment Shader
        self.fragment_shader = gl.glCreateShader(gl.GL_FRAGMENT_SHADER)
        gl.glShaderSource(self.fragment_shader, f_str)
        gl.glCompileShader(self.fragment_shader)

        # Shader Program
        self.program = gl.glCreateProgram()
        gl.glAttachShader(self.program, self.vertex_shader)
        gl.glAttachShader(self.program, self.fragment_shader)
        gl.glLinkProgram(self.program)

        # Clean up
        gl.glDeleteShader(self.vertex_shader)
        gl.glDeleteShader(self.fragment_shader)

        vertices = (
            1.0,  1.0, 0.0, 1.0, 1.0,
            1.0, -1.0, 0.0, 1.0, 0.0,
            -1.0, -1.0, 0.0, 0.0, 0.0,
            -1.0, -1.0, 0.0, 0.0, 0.0,
            -1.0,  1.0, 0.0, 0.0, 1.0,
            1.0,  1.0, 0.0, 1.0, 1.0
        )
        vertices = np.array(vertices, dtype=np.float32)

        self.vao = gl.glGenVertexArrays(1)
        self.vbo = gl.glGenBuffers(1)

        # Bind VAO
        gl.glBindVertexArray(self.vao)

        # Bind VBO
        gl.glBindBuffer(gl.GL_ARRAY_BUFFER, self.vbo)
        gl.glBufferData(gl.GL_ARRAY_BUFFER, 4 * len(vertices), vertices,
                        gl.GL_STATIC_DRAW)

        stride = 5 * 4
        gl.glVertexAttribPointer(0, 3, gl.GL_FLOAT, gl.GL_FALSE,
                                 stride, None)
        gl.glEnableVertexAttribArray(0)
        gl.glVertexAttribPointer(1, 2, gl.GL_FLOAT, gl.GL_FALSE,
                                 stride, ctypes.c_void_p(12))
        gl.glEnableVertexAttribArray(1)

    def resizeGL(self, w, h):
        gl.glViewport(0, 0, w, h)

    def paintGL(self):
        gl.glClearColor(0.5, 0, 0, 1.0)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        gl.glUseProgram(self.program)
        gl.glBindVertexArray(self.vao)
        gl.glDrawArrays(gl.GL_TRIANGLES, 0, 6)
