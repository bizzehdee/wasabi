#include <precomp.h>
#include "glbuffer.h"
#include <wnd/bltcanvas.h>
#include <draw/bitmap.h>
#include <bfc/std_wnd.h>

GLBuffer::GLBuffer() {
  m_contextcanvas = NULL;
  m_depthbits = 16;

  m_wnd = NULL;
  m_wnddc = NULL;
  m_wndrc = NULL;
  m_pbuffer = NULL;
  m_pbufferdc = NULL;
  m_pbuffercontext = NULL;
}

GLBuffer::~GLBuffer() {
  deleteContext();
  deleteBuffer();
  m_texturefiles.deleteAll();
}

LRESULT CALLBACK glBufferDummyWndProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam) {
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GLBuffer::createContext() {
  if (!m_contextcanvas) return;
  deleteContext();

  int iwidth, iheight, ip;
  m_contextcanvas->getDim(&iwidth, &iheight, &ip);

  // First create a dummy OpenGL window -- this is only necessary to obtain a DC to a HW-accelerated GL capable window.

  m_wnd = StdWnd::createWnd(0, 0, iwidth, iheight, TRUE, 0, NULL, WASABI_API_APP->main_gethInstance(), "GLBUFFERDUMMY", glBufferDummyWndProc, NULL);
  m_wnddc = GetDC(m_wnd);

  PIXELFORMATDESCRIPTOR pfd;
  MEMSET(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cDepthBits = getDepthBits();
  pfd.cColorBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;

  int baseformat = ChoosePixelFormat(m_wnddc, &pfd);
  if (baseformat == 0 || !SetPixelFormat(m_wnddc, baseformat, &pfd) || ((m_wndrc = wglCreateContext(m_wnddc)) == NULL)) {
	  ReleaseDC(m_wnd, m_wnddc);
	  StdWnd::destroyWnd(m_wnd);
    m_wnd = NULL;
    m_wnddc = NULL;
    DebugString("GLBuffer could not create an OpenGL window");
    return;
  }

  wglMakeCurrent(m_wnddc, m_wndrc);

  GLeeInit();

	int pf_attr[] =
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE,       // P-buffer will be used with OpenGL
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // Enable render to p-buffer
		//WGL_BIND_TO_TEXTURE_RGBA_ARB, TRUE, // P-buffer will be used as a texture
		WGL_RED_BITS_ARB, 8,                // At least 8 bits for RED channel
		WGL_GREEN_BITS_ARB, 8,              // At least 8 bits for GREEN channel
		WGL_BLUE_BITS_ARB, 8,               // At least 8 bits for BLUE channel
		WGL_ALPHA_BITS_ARB, 8,              // At least 8 bits for ALPHA channel
		WGL_DEPTH_BITS_ARB, 16,             // At least 16 bits for depth buffer
		WGL_DOUBLE_BUFFER_ARB, FALSE,       // We don't require double buffering
		0                                   // Zero terminates the list
	};

  // Now obtain a list of pixel formats that meet these minimum
  // requirements.
  int format;
  unsigned int nformats;
  if ( !wglChoosePixelFormatARB(m_wnddc, pf_attr, NULL, 1, &format, &nformats ) || nformats == 0) {
    DebugString("GLBuffer could not find a suitable pixel format" );
    return;
  }

  int iattribs[4] = {WGL_PBUFFER_LARGEST_ARB, GL_FALSE, 0, 0};

	m_pbuffer = wglCreatePbufferARB(m_wnddc, format, iwidth, iheight, iattribs);
  m_pbufferdc = wglGetPbufferDCARB(m_pbuffer);
  m_pbuffercontext = wglCreateContext(m_pbufferdc);

  if (!m_pbuffer || !m_pbufferdc || !m_pbuffercontext) {
    deleteContext();
    DebugString("GLBuffer could not create a pbuffer");
    return;
  }

  makeCurrent();
 
  loadTextures();
}

int GLBuffer::createBuffer(int w, int h) {
  if (m_contextcanvas) {
    int cw, ch, cp;
    m_contextcanvas->getDim(&cw, &ch, &cp);
    if (cw == w && ch == h) return 0;
  }
  deleteContext();
  deleteBuffer();
  m_contextcanvas = new BltCanvas(w, h);
  createContext();
  makeCurrent();
  return 1;
}

void GLBuffer::deleteContext() {
  if (m_pbuffercontext) {
    deleteTextures();
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(m_pbuffercontext);
    wglReleasePbufferDCARB(m_pbuffer, m_pbufferdc);
    wglDestroyPbufferARB(m_pbuffer);
    m_pbuffercontext = NULL;
  }
  if (m_wnd) {
    ReleaseDC(m_wnd, m_wnddc);
    wglDeleteContext(m_wndrc);
    StdWnd::destroyWnd(m_wnd);
    m_wnd = NULL;
    m_wnddc = NULL;
    m_wndrc = NULL;
  }
}

void GLBuffer::deleteBuffer() {
  delete m_contextcanvas;
  m_contextcanvas = NULL;
}

void GLBuffer::addTexture(const char *filename) {
  m_texturefiles.addItem(new String(filename));
  if (m_pbuffercontext) m_textures.addItem(loadTexture(filename));
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
  return m_textures.enumItem(n);
}

GLuint GLBuffer::loadTexture(const char *filename) {
  if (m_pbuffercontext) {
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
  if (!m_pbuffercontext) {
    DebugString("Trying to make GLBuffer context current, but it is invalid, further code may act upon wrong context");
    return;
  }
  if (wglGetCurrentContext() == m_pbuffercontext) return;
  wglMakeCurrent(m_pbufferdc, m_pbuffercontext);
}

HGLRC GLBuffer::getContext() {
  return m_pbuffercontext;
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

void GLBuffer::commitGlToBuffer(ARGB32 *bits) {
  if (!m_pbuffercontext || !m_contextcanvas) return;
  makeCurrent();
  DWORD starttime = GetTickCount();
  for (int i=0;i<100;i++) {
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, bits);
  }
  DWORD timenow = GetTickCount();
  float n = (100.0f/((float)(timenow - starttime)/1000.0f));
  DebugString("OpenGL Hardware to RAM Transfer Test : %.2ffps", n);
}

