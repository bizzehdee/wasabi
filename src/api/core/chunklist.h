#ifndef _CHUNKLIST_H
#define _CHUNKLIST_H

#include <bfc/ptrlist.h>
#include <bfc/string/string.h>
#include <bfc/dispatch.h>
#include <bfc/foreach.h>

class ChunkInfo {
public:
  ChunkInfo(const char *pname, int pvalue) : name(pname),value(pvalue) { }
  const char *getName() const { return name; }
  int getValue() const { return value; }
  void setValue(int v) { value = v; }
private:
  String name;
  int value;
};

class NOVTABLE ChunkInfos : public Dispatchable {
protected:
  ChunkInfos() {} // protect constructor

public:
  virtual ~ChunkInfos() { }
  void addInfo(const char *info, int value) { _voidcall(ADDINFO,info,value); }
  int getInfo(const char *info) { return _call(GETINFO,0,info); }
  const char *enumInfoName(int nb) { return _call(ENUMINFONAME,"",nb); }
  int enumInfoValue(int nb) { return _call(ENUMINFOVALUE,0,nb); }
  int getNumInfos() { return _call(GETNUMINFOS,0); }

  enum {
    ADDINFO=10,
    GETINFO=20,

    ENUMINFONAME=100,
    ENUMINFOVALUE=110,

    GETNUMINFOS=200,
  };
};

class ChunkInfosI: public ChunkInfos {
public:
  ChunkInfosI() { }
  ChunkInfosI(ChunkInfos *other) {
    if(other) {
      int len=other->getNumInfos();
      for(int i=0;i<len;i++)
        addInfo(other->enumInfoName(i),other->enumInfoValue(i));
    }
  }
  virtual ~ChunkInfosI() { m_infolist.deleteAll(); }

  virtual void addInfo(const char *info, int value) {
    // modify in place if we had it
    foreach(m_infolist)
      if (!STRICMP(m_infolist.getfor()->getName(), info)) {
        m_infolist.getfor()->setValue(value);
        return;
      }
    endfor
    // otherwise add a new record
    m_infolist.addItem(new ChunkInfo(info,value));
  }
  virtual int getInfo(const char *info) { 
    for(int i=0;i<m_infolist.getNumItems();i++)
      if(!STRICMP(m_infolist[i]->getName(),info)) return m_infolist[i]->getValue();
    return 0;
  }
  virtual const char *enumInfoName(int nb) {
    if(nb>m_infolist.getNumItems()-1) return "";
    return m_infolist[nb]->getName();
  }
  virtual int enumInfoValue(int nb) {
    if(nb>m_infolist.getNumItems()-1) return 0;
    return m_infolist[nb]->getValue();
  }
  virtual int getNumInfos() { return m_infolist.getNumItems(); }
  
private:
  PtrList<ChunkInfo> m_infolist;

#undef CBCLASS
#define CBCLASS ChunkInfosI
START_DISPATCH;
  VCB(ADDINFO, addInfo);
  CB(GETINFO,getInfo);
  CB(ENUMINFONAME,enumInfoName);
  CB(ENUMINFOVALUE,enumInfoValue);
  CB(GETNUMINFOS,getNumInfos);
END_DISPATCH;
#undef CBCLASS
};

class NOVTABLE Chunk : public Dispatchable {
protected:
  Chunk() {} // protect constructor

public:
  virtual ~Chunk() { }

  const char *getType() { return _call(GETTYPE,""); }
  void *getData() { return _call(GETDATA,(void *)0); }
  int getSize() { return _call(GETSIZE,0); }
  int getInfo(const char *info) { return _call(GETINFO,0,info); }
  ChunkInfos *getChunkInfos() { return _call(GETCHUNKINFOS,(ChunkInfos *)NULL); }

  enum {
    GETTYPE=10,
    GETDATA=20,
    GETSIZE=30,
    ADDINFO=40,
    GETINFO=50,
    GETCHUNKINFOS=60,
  };
};

class ChunkI : public Chunk {
public:
  ChunkI(const char *ptype, void *pdata, int psize, ChunkInfos *minfos=NULL) : type(ptype), data(pdata), 
    size(psize), infos(minfos) { }
  virtual ~ChunkI() { delete(infos); }

  virtual const char *getType() { return type; }
  virtual void *getData() { return data; }
  virtual int getSize() { return size; }
  virtual int getInfo(const char *info) { if(infos) return infos->getInfo(info); return 0; }
  virtual ChunkInfos *getChunkInfos() { return infos; }

protected:
#undef CBCLASS
#define CBCLASS ChunkI
START_DISPATCH;
  CB(GETTYPE, getType);
  CB(GETDATA, getData);
  CB(GETSIZE, getSize);
  CB(GETINFO, getInfo);
  CB(GETCHUNKINFOS, getChunkInfos);
END_DISPATCH;
#undef CBCLASS

  String type;
  void *data;
  int size;
  ChunkInfos *infos;
};

class NOVTABLE ChunkList : public Dispatchable {
protected:
  ChunkList() {} // protect constructor

public:
  virtual ~ChunkList() { }

  int setChunk(const char *type, void *data, int size, ChunkInfos *infos=NULL) { return _call(SETCHUNK,0,type,data,size,infos); }
  Chunk *getChunk(const char *type) { return _call(GETCHUNK,(Chunk *)0,type); }
  int delChunk(const char *type) { return _call(DELCHUNK,0,type); }
  void delAllChunks() { _voidcall(DELALLCHUNKS); }
  int setChunkInfo(int chunknb, const char *info, int value) { return _call(SETCHUNKINFO,0,chunknb,info,value); }

  enum {
    SETCHUNK=10,
    GETCHUNK=20,
    DELCHUNK=30,
    DELALLCHUNKS=40,
    SETCHUNKINFO=50,
  };
};

class ChunkListI : public ChunkList {
public:
  ChunkListI();
  virtual ~ChunkListI();

  virtual int setChunk(const char *type, void *data, int size, ChunkInfos *infos=NULL);
  virtual Chunk *getChunk(const char *type);
  virtual int delChunk(const char *type);
  virtual void delAllChunks();
  virtual int setChunkInfo(int chunknb, const char *info, int value);

protected:
  RECVS_DISPATCH;

  PtrList<Chunk> m_chunklist;
};

#endif
