#include <precomp.h>
#include <setjmp.h>
#include <bfc/assert.h>
#include <api/api.h>

#include <bfc/std.h>
#ifdef WASABI_COMPILE_SKIN
#include <api/skin/skinfilter.h>	// ApplySkinFilters
#endif

#include "imgldr.h"
#include <img/winbmp.h>
#include <img/skinbmps.h>
#include <zlib/unzip.h>

#include <api/api.h>
#ifdef WASABI_COMPILE_SKIN
#include <api/skin/skinparse.h>
#include <api/skin/skinelem.h>
#include <api/skin/gammamgr.h>
#endif

#ifdef WASABI_COMPILE_SKIN
#include <api/service/svcs/svc_skinfilter.h>
#endif
#include <api/service/svcs/svc_imgload.h>
#include <api/service/svcs/svc_imggen.h>

//#define DEBUG_OUTPUT

#define IMAGEHEADERLEN 256

ARGB32 *imageLoader::makeBmp(OSMODULEHANDLE hInst, int id, int *has_alpha, int *w, int *h, const char *forcegroup) {
  ARGB32 *bits=makeBmp(StringPrintf("res://%u,%i",hInst,id), NULL, has_alpha, w,h, NULL, TRUE, NULL);
  if (bits && *w > 0 && *h > 0) {
#ifdef WASABI_COMPILE_SKIN
    ApplySkinFilters::apply(StringPrintf("resource:%x,%d", hInst, id), forcegroup, bits, *w, *h);
#endif
  }
  return bits;
}

String imageLoader::getWallpaper() {
  String ret("");
#ifdef WIN32
  HKEY hkey;
  static char file[WA_MAX_PATH];
  file[0]=0;
  if (RegOpenKey(HKEY_CURRENT_USER, "Control Panel\\Desktop", &hkey) == ERROR_SUCCESS)
  {
    unsigned long len=sizeof(file);
    RegQueryValueEx(hkey, "Wallpaper", 0, NULL, (unsigned char *)&file, &len);
    RegCloseKey(hkey);
  }
  if (file[0] && GetFileAttributes(file)!=(DWORD)-1) ret = file;
#endif
  return ret;
}


ARGB32 *imageLoader::makeBmp(const char *_filename, const char *path, int *has_alpha, int *w, int *h, XmlReaderParams *params, BOOL addMem, int *force_nocache) {
  String filename(_filename);
  ARGB32 *ret = NULL;

  if (has_alpha != NULL) *has_alpha = 0;	//BU

  if (STRCASEEQL(filename, "$wallpaper"))
    filename = getWallpaper();

  // test image generator services FIRST
  ImgGeneratorEnum ige(filename);
  svc_imageGenerator *gen;
  while ((gen = ige.getNext()) != NULL) {
    ret = gen->genImage(filename, has_alpha, w, h, params);
    int cacheable = gen->outputCacheable();
    ige.release(gen);
    if (ret != NULL) {
#ifdef WASABI_COMPILE_SKIN
      ApplySkinFilters::apply(params->getItemValue("id"), params->getItemValue("gammagroup"), ret, *w, *h);
#endif
      if (addMem) addMemUsage(filename, (*w) * (*h) * sizeof(int));
      optimizeHasAlpha(ret, *w * *h, has_alpha);
      // don't try to cache generated images
      if (force_nocache) *force_nocache = !cacheable;
      return ret;
    }
  }

  FILE *in=NULL;
  MemBlock<unsigned char> mem;

  char olddir[WA_MAX_PATH];
  Std::getCurDir(olddir, WA_MAX_PATH-1);
  Std::setCurDir(WASABI_API_APP->path_getAppPath());

  String file;
  if (path)
    in = FOPEN(file.printf("%s%s", path, filename.v()), "rb");
  else
    in = FOPEN(file.printf("%s", filename), "rb");
#ifdef WASABI_COMPILE_SKIN
  if (!in)
    in = FOPEN(file.printf("%s%s",WASABI_API_SKIN->getSkinPath(),filename.v()), "rb");
  if (!in)
    in = FOPEN(file.printf("%s%s",Skin::getDefaultSkinPath(),filename.v()) ,"rb");
#endif
  if (!in && path) {
    in = FOPEN(file = filename.v(),"rb");
  }
  if (!in) { // look in cur dir
    in = FOPEN(file.printf("%s%s%s", olddir, DIRCHARSTR, filename), "rb");
  }

  Std::setCurDir(olddir);

  if (in) {
    int filelen = FGETSIZE(in);
    if (filelen > 0) {
      mem.setSize(filelen);
      int len = FREAD(mem, 1, mem.getSizeInBytes(), in);
      if (len == filelen) {
        svc_imageLoader *loader = ImgLoaderEnum(mem, len).getNext();
        if (loader != NULL) {
          ret = loader->loadImage(mem, mem.getSizeInBytes(), w, h, params);
          if (ret != NULL) {
            if (addMem) addMemUsage(file, (*w) * (*h) * sizeof(ARGB32));
            optimizeHasAlpha(ret, *w * *h, has_alpha);
          }
          SvcEnum::release(loader);
        }
      }
    } // filelen > 0
    FCLOSE(in);
  } // if file opened
  if (ret != NULL) {
    return ret;
  } else if(in) {

    int m = getFileType(mem);

#ifdef WIN32
    switch (m) {
      case FT_BMP: {
      
      char tmpname[WA_MAX_PATH]="";

      // FG> if loading bmp from disk, no need to do the copy to disk

      HBITMAP hbmp = (HBITMAP)LoadImage(0, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

      if (!hbmp) {

		  // CT> extract/copy the file into temp directory (so we don't have any trouble if the file
		  // is in a ZIP file). this whole copying thing will go away as soon as we'll get rid of 
		  // the LoadImage win32 function and use our own bmp loading functions

        GetTempPath(WA_MAX_PATH,tmpname);
        STRCAT(tmpname,"wa3tmp");
        FILE *fs=FOPEN(file,"rb");
        if(fs) {
          FILE *fd=FOPEN(tmpname,"wb");
          int l;
          do {
            char buf[1024];
            l=FREAD(buf,1,sizeof(buf),fs);
            if (l > 0) FWRITE(buf,1,l,fd);
          } while (l > 0);
          FCLOSE(fs);
          FCLOSE(fd);
          hbmp = (HBITMAP)LoadImage(0, tmpname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
        }
        if(!hbmp) {
#ifdef WASABI_COMPILE_SKIN_WA2
          // bitmap not found or broken (like in the netscape skin)
          // try to find it in the Classic skin (wa2)
          fs=FOPEN(StringPrintf("%sClassic/%s",WASABI_API_APP->getSkinsPath(),filename.v()),"rb");
          if(fs) {
            FILE *fd=FOPEN(tmpname,"wb");
            int l;
            do {
              char buf[1024];
              l=FREAD(buf,1,sizeof(buf),fs);
              if (l > 0) FWRITE(buf,1,l,fd);
            } while (l > 0);
            FCLOSE(fs);
            FCLOSE(fd);
            hbmp = (HBITMAP)LoadImage(0, tmpname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
          }
#endif //WASABI_COMPILE_SKIN_WA2
        }
        if(!hbmp) {
          // no luck :(
          unlink(tmpname);
          return 0;
        }
      }

        BITMAP bm;
        HDC hMemDC, hMemDC2;
        HBITMAP hprev,hprev2;
        HBITMAP hsrcdib;
        void *srcdib;
        BITMAPINFO srcbmi={0,};
        int r = GetObject(hbmp, sizeof(BITMAP), &bm);
        ASSERT(r != 0);
        *w=bm.bmWidth;
        *h=ABS(bm.bmHeight);

        ARGB32 *newbits;
        srcbmi.bmiHeader.biSize=sizeof(srcbmi.bmiHeader);
        srcbmi.bmiHeader.biWidth=*w;
        srcbmi.bmiHeader.biHeight=-*h;
        srcbmi.bmiHeader.biPlanes=1;
        srcbmi.bmiHeader.biBitCount=32;
        srcbmi.bmiHeader.biCompression=BI_RGB;

        hMemDC = CreateCompatibleDC(NULL);
        hMemDC2 = CreateCompatibleDC(NULL);
          hsrcdib=CreateDIBSection(hMemDC,&srcbmi,DIB_RGB_COLORS,&srcdib,NULL,0);
          ASSERTPR(hsrcdib != 0, "CreateDIBSection() failed #69");
              hprev2 = (HBITMAP) SelectObject(hMemDC2, hbmp);
              hprev = (HBITMAP) SelectObject(hMemDC, hsrcdib);
                BitBlt(hMemDC,0,0,*w,*h,hMemDC2,0,0,SRCCOPY);
                newbits=(ARGB32*)MALLOC_NOINIT((*w)*(*h)*sizeof(ARGB32));
                MEMCPY32(newbits,srcdib,(*w)*(*h));
                {
                  // put the alpha channel to 255
                  unsigned char *b=(unsigned char *)newbits;
                  int l=(*w)*(*h);
                  for(int i=0;i<l;i++)
                    b[(i*4)+3]=0xff;
                }
              SelectObject(hMemDC, hprev);
              SelectObject(hMemDC2, hprev2);
          DeleteObject(hsrcdib);
        DeleteDC(hMemDC2);
        DeleteDC(hMemDC);

        DeleteObject(hbmp);
     
        if (tmpname[0]) unlink(tmpname);	// destroy temp extraction

        if (addMem) addMemUsage(file, (*w)*(*h)*4);
        return newbits;
      }
    }
#endif
  }

  return ret;
}


int imageLoader::getFileType(unsigned char *pData)
{
  // Bmp ?
#ifdef WIN32
  WINBITMAPFILEHEADER * pBFH;
  pBFH = (WINBITMAPFILEHEADER *) pData;
	#ifdef WASABI_PLATFORM_WIN32
  if (pBFH->bfType == 0x4d42) 
	#else
  if (pBFH->bfType == 0x424d)
	#endif
    return FT_BMP;
#endif
  return FT_UNKNOWN;
}

#ifdef WASABI_COMPILE_SKIN

ARGB32 *imageLoader::requestBitmap(const char *id, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached) { 
  int d;

  XmlReaderParams *params=NULL;
  const char *rootpath=NULL;

  const char *aliastarget=SkinElementsMgr::getElementAlias(id);
  if (aliastarget) id = aliastarget;

  const char *efile=SkinElementsMgr::getBitmapFilename(id, x, y, subw, subh, &d, &rootpath, &params);

  if (x && *x == -1) *x = 0;
  if (y && *y == -1) *y = 0;

  if (!efile) efile = id;

  if (cached) {
    String f = rootpath;
    if (f.notempty() && !Std::isDirChar(f.lastChar())) f.cat(DIRCHARSTR);
    f.cat(efile);
    f.toupper();
    f.changeChar('\\','/');

    int pos=-1;
    skinCacheEntry *entry=skinCacheList.findItem(f.getValue(),&pos);
    if(pos!=-1) {
      //find first one
      while(pos>0 && !STRCMP(skinCacheList[pos-1]->fullpathfilename,f)) pos--;
      do {
        skinCacheEntry *entry=skinCacheList[pos];
        if (GammaMgr::gammaEqual(entry->original_element_id, id) && layerEqual(entry->original_element_id, id)) {
          entry->usageCount++;
          if (w) *w=entry->width;
          if (h) *h=entry->height;
          if (has_alpha) *has_alpha = entry->has_alpha;
          return entry->bitmapbits;
        }
        pos++;
        if(pos>=skinCacheList.getNumItems()) break;
      } while(!STRCMP(skinCacheList[pos]->fullpathfilename,f));
    }
  }
  
/*
#ifdef _DEBUG

	BitmapElementId id = Skin::getBitmapId(file);
	if (id == SKIN_BITMAP_UNKNOWN) {
	  DebugString("Note: %s not in BitmapElement list\n", file);
	}
#endif	
*/
  
  int force_nocache = 0;
  int t_has_alpha = 0;
  ARGB32 *bits=makeBmp(efile, rootpath, &t_has_alpha, w, h, params, TRUE, &force_nocache);
  if (has_alpha != NULL) *has_alpha = t_has_alpha;

  if (!bits) return NULL;

  if (force_nocache || !cached) return bits;

  skinCacheEntry *cachedbmp=new skinCacheEntry;

  if (params) {
    cachedbmp->params = new XmlReaderParamsI;
    for (int i=0;i<params->getNbItems();i++)
      cachedbmp->params->addItem(params->getItemName(i), params->getItemValue(i));
  } else {
    cachedbmp->params = NULL;
  }

  cachedbmp->usageCount = 1;
  cachedbmp->bitmapbits = bits;
  cachedbmp->filename = efile;
  cachedbmp->has_alpha = t_has_alpha;
  cachedbmp->width=*w;
  cachedbmp->height=*h;
  cachedbmp->original_element_id = id;

  //needed for findItem above
  String b = rootpath;
  if ( b.notempty() && !Std::isDirChar(b.lastChar())) b.cat(DIRCHARSTR);
  b.cat(efile);
  b.toupper();
  b.changeChar('\\','/');
  cachedbmp->fullpathfilename=b;

#ifdef WASABI_COMPILE_SKIN
  applySkinFilters(cachedbmp);
#endif
  skinCacheList.addItem(cachedbmp);

  return cachedbmp->bitmapbits;
}

int imageLoader::paramsMatch(XmlReaderParams *a, XmlReaderParams *b) {
  if (!a && !b) return 1;
  if ((!a && b) || (!b && a)) return 0;
  for (int i=0;i<a->getNbItems();i++) {
    const char *name = a->getItemName(i);
    if (STRCASEEQL(name, "w") || STRCASEEQL(name, "h") || STRCASEEQL(name, "x") || STRCASEEQL(name, "y")) continue;
    if (!STRCASEEQL(a->getItemValue(i), b->getItemValue(name)))
      return 0;
  }
  return 1;
}

int imageLoader::layerEqual(const char *id1, const char *id2) {
  int a = SkinElementsMgr::getLayerFromId(id1);
  int b = SkinElementsMgr::getLayerFromId(id2);
  return (a == b);
}

int imageLoader::getMemUsage() { return totalMemUsage; }

int imageLoader::getNumCached() { return skinCacheList.getNumItems(); }

void imageLoader::releaseBitmap(ARGB32 *bitmapbits) { //FG
  int i;
  // TODO: add critical sections

  int ni=skinCacheList.getNumItems();
  for (i=0;i<ni;i++) {
    skinCacheEntry *entry = skinCacheList.enumItem(i);
    if (entry->bitmapbits == bitmapbits) {
      entry->usageCount--;
      if (entry->usageCount == 0) {
        subMemUsage(entry->width*entry->height*sizeof(int));
        FREE(entry->bitmapbits);
        delete entry->params;
        skinCacheList.removeByPos(i);
        delete entry;
        if (skinCacheList.getNumItems() == 0) skinCacheList.removeAll();
      }
      return;
    }
  }
// bitmap was not a cached skin bitmap, simply free it
  release(bitmapbits);
}

void imageLoader::retainBitmap(ARGB32 *bitmapbits) { 
  int i;
  int ni=skinCacheList.getNumItems();
  for (i=0;i<ni;i++) {
    skinCacheEntry *entry = skinCacheList.enumItem(i);
    if (entry->bitmapbits == bitmapbits) {
      entry->usageCount++;
      return;
    }
  }
}

#endif //WASABI_COMPILE_SKIN

void imageLoader::release(ARGB32 *bitmapbits) { 
  FREE(bitmapbits);
}

void imageLoader::optimizeHasAlpha(ARGB32 *bits, int len, int *has_alpha) {
  if (len <= 0 || has_alpha == NULL) return;
  for (*has_alpha = 0; len; len--, bits++) {
    ARGB32 v = *bits;
    unsigned int alpha = v >> 24;
    if (alpha != 255) {
      *has_alpha = 1;
      break;
    }
  }       
}


#ifdef WASABI_COMPILE_SKIN

void imageLoader::applySkinFilters() { //FG
  int i;

  Skin::unloadResources();
  for (i=0; i<skinCacheList.getNumItems(); i++) {
    skinCacheEntry *entry = skinCacheList.q(i);
    applySkinFilters(entry);
  }

  Skin::newSkinPart();
  Skin::reloadResources();
}

void imageLoader::applySkinFilters(skinCacheEntry *entry) {
  ASSERT(entry != NULL);

  int w = entry->width, h = entry->height;

#ifdef WASABI_COMPILE_SKIN
  ApplySkinFilters::apply(entry->original_element_id, NULL, (ARGB32*)entry->bitmapbits, w, h);
#endif
}

COLORREF imageLoader::filterSkinColor(COLORREF color, const char *element_id, const char *groupname) {

  SkinFilterEnum sfe;

  svc_skinFilter *obj;
  while (1) {
    obj = sfe.getNext(FALSE);    
    if (!obj) break;
    color = obj->filterColor(color, element_id, groupname);
    sfe.getLastFactory()->releaseInterface(obj);
  }

  return color;
}

#endif //WASABI_COMPILE_SKIN

void imageLoader::addMemUsage(const char *filename, int size) {
  totalMemUsage += size;
#ifdef DEBUG_OUTPUT
  DebugString("Bitmaps memory usage : %s - %d\n", filename, totalMemUsage);
#endif
}
void imageLoader::subMemUsage(int size) {
  totalMemUsage -= size;
}

#ifdef WASABI_COMPILE_SKIN

//PtrList<skinCacheEntry> imageLoader::skinCacheList;
PtrListInsertMultiSorted<skinCacheEntry,skinCacheComp> imageLoader::skinCacheList;
#endif //WASABI_COMPILE_SKIN

int imageLoader::totalMemUsage=0;
