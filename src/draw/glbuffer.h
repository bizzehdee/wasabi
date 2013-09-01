#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <gl/gl.h>
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
  void reset();

private:
  void createContext();
  void deleteContext();
  int createBuffer(int w, int h);
  void deleteBuffer();
  void loadTextures();
  GLuint loadTexture(const char *filename);
  void doReset(int w, int h);

  BltCanvas *m_contextcanvas;
  HGLRC m_rendercontext;
  TList<GLuint> m_textures;
  PtrList<String> m_texturefiles;
  int m_depthbits;
};

#endif
