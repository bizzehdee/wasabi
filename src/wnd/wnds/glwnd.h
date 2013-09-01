#ifndef GLWND_H
#define GLWND_H

#include <wnd/basewnd.h>
#include <glee.h>
#include <gl/gl.h>

#define GLWND_PARENT BaseWnd

class GLWnd : public GLWND_PARENT {
public:
  GLWnd();
  virtual ~GLWnd();

  HGLRC glGetContext();
  void glMakeCurrent();
  GLuint glLoadTexture(const char *filename);
  void glDeleteTextures();
  void setDoubleBuffered(int db);
  void setWaitVerticalRetrace(int waitvertical);
  void setWantMultisampling(int wms);
  void swapBuffers();

  virtual int onInit();
  virtual int onPaint(Canvas *c);
  virtual int wantEraseBackground() { return 0; }

  virtual int isMultiSamplingEnabled() { return m_multisampling; }

private:
  void glInit();
  void glShutdown();
  void resetWaitVertical();

  HGLRC m_rendercontext;
  HDC m_dc;
  TList<GLuint> m_textures;
  int m_doublebuffered;
  int m_waitvertical;
  int m_multisampling;
  int m_wantmultisampling;
};

#endif
