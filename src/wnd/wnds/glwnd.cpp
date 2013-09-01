#include <precomp.h>
#include "glwnd.h"
#include <bfc/std_wnd.h>
#include <draw/bitmap.h>
#include <bfc/std_wnd.h>

GLWnd::GLWnd() {
  setNonBuffered(1);
  m_rendercontext = NULL;
  m_doublebuffered = 1;
  m_waitvertical = 1;
  m_wantmultisampling = 1;
  m_multisampling = 0;
}

GLWnd::~GLWnd() {
  glShutdown();
}

int GLWnd::onPaint(Canvas *c) {
  return 1;
}

void GLWnd::setDoubleBuffered(int db) {
  if (db == m_doublebuffered) return;
  m_doublebuffered = db;
  if (m_rendercontext) {
    glShutdown();
    glInit();
  }
}

HGLRC GLWnd::glGetContext() {
  return m_rendercontext;
}

void GLWnd::glMakeCurrent() {
  if (!m_rendercontext) {
    DebugString("Trying to make GLWnd context current, but it is invalid, further code may act upon wrong context");
    return;
  }
  wglMakeCurrent(m_dc, m_rendercontext);
}

int GLWnd::onInit() {
  GLWND_PARENT::onInit();
  glInit();
  return 1;
}

LRESULT CALLBACK wglDummyWndProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM  lParam) {
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GLWnd::glInit() {
  if (m_rendercontext || m_dc) glShutdown();

  OSWINDOWHANDLE tmpwnd = NULL;
  HDC tmpdc = NULL;

  tmpwnd = StdWnd::createWnd(0, 0, 10, 10, TRUE, 0, NULL, WASABI_API_APP->main_gethInstance(), "WGLDUMMY", wglDummyWndProc, NULL);
  tmpdc = GetDC(tmpwnd);

  PIXELFORMATDESCRIPTOR pfd;
  MEMSET(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL|(m_doublebuffered?PFD_DOUBLEBUFFER:0)|PFD_DRAW_TO_WINDOW|PFD_GENERIC_ACCELERATED;
  pfd.iPixelType = PFD_TYPE_RGBA;
  // todo: make this configurable
  pfd.cColorBits = 24;
  pfd.cDepthBits = 16;
  pfd.cAlphaBits = 8;
  pfd.cStencilBits = 16;
  pfd.iLayerType = PFD_MAIN_PLANE;

  HGLRC tmprc;
  int baseformat = ChoosePixelFormat(tmpdc, &pfd);
  if (baseformat == 0 || !SetPixelFormat(tmpdc, baseformat, &pfd) || ((tmprc = wglCreateContext(tmpdc)) == NULL)) {
	  ReleaseDC(tmpwnd, tmpdc);
	  StdWnd::destroyWnd(tmpwnd);
    // could not even create a simple opengl window
    return;
  }
  int reqformat = baseformat;
  if (m_wantmultisampling) {
    wglMakeCurrent(tmpdc, tmprc);
    m_multisampling = 0;
    if (GLeeInit()) {
      if (GLEE_ARB_multisample) {
        int pixelFormat;
        BOOL bStatus;
        UINT numFormats;
        float fAttributes[] = {0,0};
        int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
	                            WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
	                            WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
	                            WGL_COLOR_BITS_ARB,24,
	                            WGL_ALPHA_BITS_ARB,8,
	                            WGL_DEPTH_BITS_ARB,16,
	                            WGL_STENCIL_BITS_ARB,0,
	                            WGL_DOUBLE_BUFFER_ARB, m_doublebuffered?GL_TRUE:GL_FALSE,
	                            WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
	                            WGL_SAMPLES_ARB,4,
	                            0,0};
    
        bStatus = wglChoosePixelFormatARB(tmpdc,iAttributes,fAttributes,1,&pixelFormat,&numFormats);
        if ((bStatus == GL_TRUE) && (numFormats >= 1)) {
          m_multisampling = 1;
	        reqformat = pixelFormat;
        } else {
          // ok that failed, try using 2 samples now instead of 4
          iAttributes[19] = 2;
          bStatus = wglChoosePixelFormatARB(tmpdc,iAttributes,fAttributes,1,&pixelFormat,&numFormats);
          if ((bStatus == GL_TRUE) && (numFormats >= 1)) {
            m_multisampling = 1;
  	        reqformat = pixelFormat;	  
          }
        }
      }
    }
  }

  wglMakeCurrent(NULL, NULL);
  ReleaseDC(tmpwnd, tmpdc);
  wglDeleteContext(tmprc);
  StdWnd::destroyWnd(tmpwnd);

  m_dc = StdWnd::getDC(gethWnd());

  SetPixelFormat(m_dc, reqformat, &pfd);

  m_rendercontext = wglCreateContext(m_dc);
  wglMakeCurrent(m_dc, m_rendercontext);

  GLeeInit();
  resetWaitVertical();
}

void GLWnd::glShutdown() {
  glDeleteTextures();
  if (m_rendercontext) {
    wglDeleteContext(m_rendercontext);
    m_rendercontext = NULL;
  }
  if (m_dc) {
    StdWnd::releaseDC(gethWnd(), m_dc);
    m_dc = NULL;
  }
}

GLuint GLWnd::glLoadTexture(const char *filename) {
  Bitmap b(filename);
  b.bgra2argb();

  glMakeCurrent();

  GLuint texture;
  glGenTextures(1, &texture); 
  glBindTexture(GL_TEXTURE_2D, texture); 
  glTexImage2D(GL_TEXTURE_2D, 0, 4, b.getWidth(), b.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, b.getBits());
  
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  m_textures.addItem(texture);
  return texture;
}

void GLWnd::glDeleteTextures() {
  glMakeCurrent();
  for (int i=0;i<m_textures.getNumItems();i++) {
    GLuint tex = m_textures.enumItem(i);
    ::glDeleteTextures(1, &tex);
  }
  m_textures.removeAll();
}

void GLWnd::swapBuffers() {
  if (m_doublebuffered) ::SwapBuffers(m_dc);
}

void GLWnd::setWaitVerticalRetrace(int waitvertical) {
  m_waitvertical = waitvertical;
  if (m_rendercontext) resetWaitVertical();
}

void GLWnd::resetWaitVertical() {
  if (GLEE_WGL_EXT_swap_control) wglSwapIntervalEXT(m_waitvertical);
}

void GLWnd::setWantMultisampling(int wms) {
  m_wantmultisampling = wms;
}
