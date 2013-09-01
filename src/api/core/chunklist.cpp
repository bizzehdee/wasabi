#include "precomp.h"
#include "chunklist.h"

#undef CBCLASS
#define CBCLASS ChunkListI
START_DISPATCH;
  CB(SETCHUNK, setChunk);
  CB(GETCHUNK, getChunk);
  CB(DELCHUNK, delChunk);
  VCB(DELALLCHUNKS, delAllChunks);
END_DISPATCH;
#undef CBCLASS

ChunkListI::ChunkListI() {
}

ChunkListI::~ChunkListI() {
  delAllChunks();
}

int ChunkListI::setChunk(const char *type, void *data, int size, ChunkInfos *infos) {
  Chunk *c;
  if(c=getChunk(type))
  {
    m_chunklist.delItem(c);
    if(!infos) {
      // copy old infos into new one
      infos=new ChunkInfosI(c->getChunkInfos());
    }
    delete(c);
  }
  m_chunklist.addItem(new ChunkI(type,data,size,infos));
  return 1;
}

Chunk *ChunkListI::getChunk(const char *type) {
  int l=m_chunklist.getNumItems();
  for(int i=0;i<l;i++)
    if(!STRICMP(m_chunklist[i]->getType(),type)) return m_chunklist[i];
  return 0;
}

int ChunkListI::delChunk(const char *type) {
  int l=m_chunklist.getNumItems();
  for(int i=0;i<l;i++)
    if(!STRICMP(m_chunklist[i]->getType(),type)) 
    {
      delete(m_chunklist[i]);
      m_chunklist.delByPos(i);
      i--; l--;
    }
  return 0;
}

void ChunkListI::delAllChunks() {
  m_chunklist.deleteAll();
}

int ChunkListI::setChunkInfo(int chunknb, const char *info, int value) {
  return 0;
}
