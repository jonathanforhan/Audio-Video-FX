"""
DOCS:
https://pyopengl.sourceforge.net/documentation/pydoc/OpenGL.GL.html
"""

from PyQt6.QtOpenGLWidgets import QOpenGLWidget
from ctypes import c_void_p
import numpy as np

from OpenGL.GL import *
from OpenGL.GL import shaders
from PIL import Image


class GLWidget(QOpenGLWidget):
    def __init__(self, parent):
        super(GLWidget, self).__init__(parent)
        self.setMinimumSize(640, 480)

    def __del__(self):
        glDeleteVertexArrays(1, np.array(self.vao))
        glDeleteBuffers(1, np.array(self.vbo))
        glDeleteProgram(self.program)

    @staticmethod
    def __init_shaders(v_path, f_path):
        with open(v_path) as f:
            v_str = f.read()

        with open(f_path) as f:
            f_str = f.read()

        v_shader = shaders.compileShader(v_str, GL_VERTEX_SHADER)
        f_shader = shaders.compileShader(f_str, GL_FRAGMENT_SHADER)

        program = 0
        try:
            program = shaders.compileProgram(v_shader, f_shader)
        except:
            glDeleteShader(v_shader)
            glDeleteShader(f_shader)
        return program
        
    @staticmethod
    def __init_buffers(vertices):
        vao = glGenVertexArrays(1)
        glBindVertexArray(vao)

        vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, vbo)
        glBufferData(GL_ARRAY_BUFFER, 4 * len(vertices), vertices,
                        GL_STATIC_DRAW)

        glVertexAttribPointer(0, 3, GL_FLOAT, False, 20, None)
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(1, 2, GL_FLOAT, False, 20, c_void_p(12))
        glEnableVertexAttribArray(1)

        glBindVertexArray(0)
        glBindBuffer(GL_ARRAY_BUFFER, 0)

        return vao, vbo

    @staticmethod
    def __init_textures(t_path, program):
        img = Image.open(t_path)
        img_data = np.array(list(img.getdata()), np.int8)
        texture = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D, texture)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glUseProgram(program)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, img_data)
        glUniform1i(glGetUniformLocation(program, "u_texture"), 0)

        return texture


    def initializeGL(self):
        glClearColor(0.5, 0, 0, 1.0)

        vertices = np.array([
            1.0,  1.0, 0.0, 1.0, 1.0,
            1.0, -1.0, 0.0, 1.0, 0.0,
            -1.0, -1.0, 0.0, 0.0, 0.0,
            -1.0, -1.0, 0.0, 0.0, 0.0,
            -1.0,  1.0, 0.0, 0.0, 1.0,
            1.0,  1.0, 0.0, 1.0, 1.0
        ], dtype=np.float32)

        self.vao, self.vbo = GLWidget.__init_buffers(vertices)

        self.program = GLWidget.__init_shaders("shaders/basic.vert",
                                               "shaders/basic.frag")

        self.texture = GLWidget.__init_textures("samples/testcard.png",
                                                self.program)

    def resizeGL(self, w, h):
        glViewport(0, 0, w, h)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)

        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.texture)

        glUseProgram(self.program)
        glBindVertexArray(self.vao)

        glDrawArrays(GL_TRIANGLES, 0, 6)
