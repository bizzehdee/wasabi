#include <precomp.h>
#include "glbuffer.h"
#include <wnd/bltcanvas.h>
#include <draw/bitmap.h>

GLBuffer::GLBuffer() {
  m_rendercontext = NULL;
  m_contextcanvas = NULL;
  m_depthbits = 16;
}

GLBuffer::~GLBuffer() {
  deleteContext();
  deleteBuffer();
  m_texturefiles.deleteAll();
}

void GLBuffer::createContext() {
  if (!m_contextcanvas) return;
  deleteContext();
  
  PIXELFORMATDESCRIPTOR pfd;
  MEMSET(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_BITMAP|PFD_SUPPORT_OPENGL|PFD_SUPPORT_GDI|PFD_GENERIC_ACCELERATED;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cRedBits = 8;
  pfd.cRedShift = 0;
  pfd.cGreenBits = 8;
  pfd.cGreenShift = 8;
  pfd.cBlueBits = 8;
  pfd.cBlueShift = 16;
  pfd.cAlphaBits = 8;
  pfd.cAlphaShift = 24;
  pfd.cDepthBits = getDepthBits();
  pfd.cAccumBits = 0;
  pfd.cAccumAlphaBits = 0;
  pfd.cStencilBits = 0;
  pfd.iLayerType = PFD_MAIN_PLANE;
  int npixformat = ChoosePixelFormat(m_contextcanvas->getHDC(), &pfd);
  SetPixelFormat(m_contextcanvas->getHDC(), npixformat, &pfd);
  DescribePixelFormat(m_contextcanvas->getHDC(), npixformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  m_rendercontext = wglCreateContext(m_contextcanvas->getHDC());
  loadTextures();
}

int GLBuffer::createBuffer(int w, int h) {
  if (m_contextcanvas) {
    int cw, ch, cp;
    m_contextcanvas->getDim(&cw, &ch, &cp);
    if (cw == w && ch == h) return 0;
  }
  doReset(w, h);
  return 1;
}

void GLBuffer::doReset(int w, int h) {
  deleteContext();
  deleteBuffer();
  m_contextcanvas = new BltCanvas(w, h);
  createContext();
  makeCurrent();
}

void GLBuffer::deleteContext() {
  if (m_rendercontext) {
    deleteTextures();
    wglDeleteContext(m_rendercontext);
    m_rendercontext = NULL;
  }
}

void GLBuffer::deleteBuffer() {
  delete m_contextcanvas;
  m_contextcanvas = NULL;
}

void GLBuffer::addTexture(const char *filename) {
  m_texturefiles.addItem(new String(filename));
  if (m_rendercontext) m_textures.addItem(loadTexture(filename));
}

void GLBuffer::deleteTextures() {
  makeCurrent();
  for (int i=0;i<m_textures.getNumItems();i++) {
    GLuint texture = m_textures.enumItem(i);
    glDeleteTextures(1, &texture);
  }
  m_textures.removeAll();
}

void GLBuffer::loadTextures() {
  deleteTextures();
  foreach(m_texturefiles)
    m_textures.addItem(loadTexture(m_texturefiles.getfor()->getValue()));
  endfor;
}

GLuint GLBuffer::getGlTextureId(int n) {
  if (n>= m_textures.getNumItems()) return 0;
  return m_textures.enumItem(n);
}

GLuint GLBuffer::loadTexture(const char *filename) {
  if (m_rendercontext) {
    Bitmap b(filename);
    b.bgra2argb();

    makeCurrent();

    GLuint texture;
    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexImage2D(GL_TEXTURE_2D, 0, 4, b.getWidth(), b.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, b.getBits());
  
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    return texture;
  }
  return -1;
}

int GLBuffer::setSize(int w, int h) {
  return createBuffer(w, h);
}

int GLBuffer::getWidth() {
  if (!m_contextcanvas) return 0;
  int cw, ch, cp;
  m_contextcanvas->getDim(&cw, &ch, &cp);
  return cw;
}

int GLBuffer::getHeight() {
  if (!m_contextcanvas) return 0;
  int cw, ch, cp;
  m_contextcanvas->getDim(&cw, &ch, &cp);
  return ch;
}

ARGB32 *GLBuffer::getBits() {
  if (!m_contextcanvas) return NULL;
  return (ARGB32 *)m_contextcanvas->getBits();
}

void GLBuffer::makeCurrent() {
  if (!m_contextcanvas || !m_rendercontext) {
    DebugString("Trying to make GLBuffer context current, but it is invalid, further code may act upon wrong context");
    return;
  }
  wglMakeCurrent(m_contextcanvas->getHDC(), m_rendercontext);
}

HGLRC GLBuffer::getContext() {
  return m_rendercontext;
}

BltCanvas *GLBuffer::getBuffer() {
  return m_contextcanvas;
}

int GLBuffer::getDepthBits() {
  return m_depthbits;
}

void GLBuffer::setDepthBits(int db) {
  m_depthbits = db;
}

void GLBuffer::reset() {
  doReset(getWidth(), getHeight());
}
