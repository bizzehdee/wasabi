#include <precomp.h>

#include "loader_jpg.h"

#include "jpgdlib/jpegdecoder.h"

#include <draw/blending.h>
#include <api/api.h>

int JpgLoad::isMine(const char *filename) {
  const char *ext = Std::extension(filename);
  if (STRCASEEQLSAFE(ext, "jpg")) return 1;
  if (STRCASEEQLSAFE(ext, "jpeg")) return 1;
  return 0;
}

int JpgLoad::testData(const void *data, int datalen) {
  const unsigned char *text = static_cast<const unsigned char *>(data);
  if (text[0] == 0xFF && text[1] == 0xD8 && text[2] == 0xFF)
    return 1;
  return 0;
}

class reader : public jpeg_decoder_stream {
public:
  reader(const void *_data, int _datalen) : data(static_cast<const char *>(_data)), datalen(_datalen), pos(0) {}
  virtual int read(uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag) {
    if (datalen - pos < max_bytes_to_read) {
      max_bytes_to_read = datalen - pos;
      *Peof_flag = TRUE;
    }
    MEMCPY(Pbuf, data+pos, max_bytes_to_read);
    pos += max_bytes_to_read;
    return max_bytes_to_read;
  }

private:
  const char *data;
  int datalen;
  int pos;
};

int JpgLoad::getDimensions(const void *data, int datalen, int *w, int *h) {
  Pjpeg_decoder_stream Pinput_stream = new reader(data, datalen);
  Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, Blenders::MMX_AVAILABLE()?TRUE:FALSE);
  if (Pd->get_error_code() != 0 || Pd->begin())
  {
    // Always be sure to delete the input stream object _after_
    // the decoder is deleted. Reason: the decoder object calls the input
    // stream's detach() method.
    delete Pd;
    delete Pinput_stream;
    return 0;
  }
  if (w) *w = Pd->get_width();
  if (h) *h = Pd->get_height();
  delete Pd;
  delete Pinput_stream;
  return 1;
}


ARGB32 *JpgLoad::loadImage(const void *data, int datalen, int *w, int *h, XmlReaderParams *params) {
  Pjpeg_decoder_stream Pinput_stream = new reader(data, datalen);

  Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, Blenders::MMX_AVAILABLE()?TRUE:FALSE);

  if (Pd->get_error_code() != 0 || Pd->begin())
  {
    // Always be sure to delete the input stream object _after_
    // the decoder is deleted. Reason: the decoder object calls the input
    // stream's detach() method.
    delete Pd;
    delete Pinput_stream;
    return NULL;
  }

  uchar *Pbuf = NULL;
#ifdef WASABI_COMPILE_MEMMGR
    Pbuf = (uchar *)WASABI_API_MEMMGR->sysMalloc(Pd->get_width() * Pd->get_height() * 4);
#else //WASABI_COMPILE_MEMMGR
    Pbuf = (uchar *)MALLOC_NOINIT(Pd->get_width() * Pd->get_height() * 4);
#endif //WASABI_COMPILE_MEMMGR

  if (!Pbuf)
  {
    delete Pd;
    delete Pinput_stream;
    return NULL;
  }

  ARGB32 *bytes=(ARGB32 *)Pbuf;
//  Pbuf+=Pd->get_width()*(Pd->get_height()-1)*4;

  int nbc=Pd->get_num_components();
  int src_bpp = Pd->get_bytes_per_pixel();

  for ( ; ; )
  {
    void *Pscan_line_ofs;
    uint scan_line_len;

    if (Pd->decode(&Pscan_line_ofs, &scan_line_len))
      break;

    switch(nbc) {
    case 3:
      {
        uchar *Psb = (uchar *)Pscan_line_ofs;
        uchar *Pdb = Pbuf;
        for (int x = Pd->get_width(); x > 0; x--, Psb += src_bpp, Pdb += 4)
        {
          Pdb[0] = Psb[2];
          Pdb[1] = Psb[1];
          Pdb[2] = Psb[0];
          Pdb[3] = 0xff;
        }
        break;
      }
    case 1:
      {
        uchar *Psb = (uchar *)Pscan_line_ofs;
        uchar *Pdb = Pbuf;
        for (int x = Pd->get_width(); x > 0; x--, Psb += src_bpp, Pdb += 4)
        {
          Pdb[0] = Psb[0];
          Pdb[1] = Psb[0];
          Pdb[2] = Psb[0];
          Pdb[3] = 0xff;
        }
      }
      break;
    }
    Pbuf+=Pd->get_width()*4;
  } 

  *w=Pd->get_width();
  *h=Pd->get_height();

  delete Pd;
  delete Pinput_stream;

  // force alpha fully on
  int n = *w * *h;
  if (n > 0) {
    ARGB32 *argb = (ARGB32*)bytes;
    while (n--) *argb++ |= 0xff000000;
  }

  return bytes;
}
