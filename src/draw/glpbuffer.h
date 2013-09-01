#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <GLee.h>
#include <gl/gl.h>
#include <gl/glu.h>
class BltCanvas;

class GLBuffer {
public:
  GLBuffer();
  virtual ~GLBuffer();

  int setSize(int w, int h);
  ARGB32 *getBits();
  BltCanvas *getBuffer();

  void makeCurrent();
  HGLRC getContext();

  void addTexture(const char *filename);
  void deleteTextures();
  int getDepthBits();
  void setDepthBits(int db);
  GLuint getGlTextureId(int n);

  int getWidth();
  int getHeight();
  
  void commitGlToBuffer(ARGB32 *bits);

private:
  void createContext();
  void deleteContext();
  int createBuffer(int w, int h);
  void deleteBuffer();
  void loadTextures();
  GLuint loadTexture(const char *filename);

  BltCanvas *m_contextcanvas;
  TList<GLuint> m_textures;
  PtrList<String> m_texturefiles;
  int m_depthbits;
  HPBUFFERARB m_pbuffer;
  HDC m_pbufferdc;
  HGLRC m_pbuffercontext;
  
  // dummy window
  OSWINDOWHANDLE m_wnd;
  HDC m_wnddc;
  HGLRC m_wndrc;
};

#endif
