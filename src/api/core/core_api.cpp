#include <precomp.h>
#include "core_api.h"

api_core *coreApi = NULL;

static int tokenindex=0;

CallbackMultiplex::CallbackMultiplex(svc_mediaCoreI *_core) : core(_core) , seq(NULL) {

}

void CallbackMultiplex::add(CoreCallback *c) {
  list.addItem(c);
}

void CallbackMultiplex::del(CoreCallback *c) {
  list.removeItem(c);
}

void CallbackMultiplex::setSequencer(ItemSequencer *_seq) {
  if(seq) seq->onNotify(SEQNOTIFY_ONDEREGISTER);
  seq = _seq;
  seq->onNotify(SEQNOTIFY_ONREGISTER);
}

void CallbackMultiplex::delSequencer(ItemSequencer *_seq) {
  if(seq == _seq) {
    seq=NULL;
    _seq->onNotify(SEQNOTIFY_ONDEREGISTER);
  }
}

ItemSequencer *CallbackMultiplex::getSequencer() {
  return seq;
}

const char *CallbackMultiplex::getCurrent() {
  return Current;
}

void CallbackMultiplex::setCurrent(String _current){ 
  Current = _current;
}

int CallbackMultiplex::_dispatch(int msg, void *retval, void **params, int nparam) {
  foreach(list)
    static_cast<Dispatchable *>(list.getfor())->_dispatch(msg,retval,params,nparam);
  endfor;

  if(seq) {
    if(msg == CCB_NOTIFY && nparam) notify_seq(*(int *)params[0]);
  }
  return 1;
}

void CallbackMultiplex::notify_seq(int msg) {
  switch(msg){
  case NEEDNEXTFILE:
    if(seq->forward()){
    Current = seq->getNextPlayItem();
    core->setNextFile(Current);
    if(core->getPlaying())core->abortCurrentSong();
    else core->start();
    }
  case NEXTFILE:
    seq->onNotify(SEQNOTIFY_ONNEXTFILE);
    break;
  case TITLECHANGE:
    seq->onNotify(SEQNOTIFY_ONTITLECHANGE);
    break;
  case STARTED:
    seq->onNotify(SEQNOTIFY_ONSTARTED);
    break;
  case STOPPED:
    seq->onNotify(SEQNOTIFY_ONSTOPPED);
    break;
  case PAUSED:
    seq->onNotify(SEQNOTIFY_ONPAUSED);
    break;
  case UNPAUSED:
    seq->onNotify(SEQNOTIFY_ONUNPAUSED);
    break;
  }
}

Core::Core(svc_mediaCoreI *_core, CoreToken tok, const char *_name) : cmul(_core), core(_core), 
  m_coretoken(tok), name(_name) {

  core->setCallback(&cmul);
}

Core::~Core() { 
  if(core) WASABI_API_SVC->service_release(core);
}

CoreToken Core::getToken() {
  return m_coretoken;
}

const char *Core::getServiceName() {
  return name;
}

int Core::_dispatch(int msg, void *retval, void **params, int nparam) {
  if(!core) return -1; 
  return static_cast<Dispatchable *>(core)->_dispatch(msg,retval,params,nparam);
}

void Core::setSequencer(ItemSequencer *_seq) {
  cmul.setSequencer(_seq);
}

void Core::delSequencer(ItemSequencer *_seq) {
  cmul.delSequencer(_seq);
}

ItemSequencer *Core::getSequencer() {
  return cmul.getSequencer();
}

void Core::addCallback(CoreCallback *cb) { 
  cmul.add(cb);
}

void Core::delCallback(CoreCallback *cb) {
  cmul.del(cb);
}
const char *Core::getCurrent() {
  return cmul.getCurrent();
}

void Core::setCurrent(String _current) { 
  cmul.setCurrent(_current);
}

const char *core_api::core_getSupportedExtensions() {
  static String res;
  res.purge();
  res = "";

  foreach(SCAextensions) {
    res += SCAextensions.getfor()->ext;
    res += "|";
  } endfor;

  res += "|";
  res.changeChar('|','\0');

  return res;

  return NULL;
}

const char *core_api::core_getExtSupportedExtensions() {
  static String res;
  res.purge();
  res = "All supported types|";

  String exts;

  foreach(SCAextensions) {
    exts += SCAextensions.getfor()->ext;
    exts += ";";
  } endfor;
  
  if(exts.len()>256) {
    res += "*";
  } else {
    res += exts;
  }

  res += "|";

  foreach(SCAextensions) {
    res += SCAextensions.getfor()->name;
    res += " (";
    res += SCAextensions.getfor()->ext;
    res += ")";
    res += "|";
    res += SCAextensions.getfor()->ext;
    res += "|";
  } endfor;

  res += "All files (*.*)|*|";
  res += "||";

  res.changeChar('|','\0');

  return res;

  return NULL;
}

Core *core_api::tokenToCore(CoreToken tok){
  foreach(cores)
    if(cores.getfor()->getToken() == tok) return cores.getfor();
  endfor
  return NULL;
}

void core_api::initDaMainCore() {
/*  if(cores.getNumItems()) return;

  String name = "Main";

  waServiceFactory *svcf = NULL;
  svc_mediaCoreI *svc = NULL;

  int	n = WASABI_API_SVC->service_getNumServices(WaSvc::MEDIACORE);

  for(int h = 0; h < n && !svc; h++) { 
    svcf = WASABI_API_SVC->service_enumService(WaSvc::MEDIACORE, h);
    svc = castService<svc_mediaCoreI>(svcf);
    if(STRCMP(svcf->getServiceName(),name)) { 
      WASABI_API_SVC->service_release(svc); 
      svc = NULL; 
    } 
  }

  if(svc) cores.addItem(new Core(svc, (CoreToken)0, name));
  */
}

CoreToken core_api::core_create() {
  //initDaMainCore(); 

  waServiceFactory *svcf = NULL;
  svc_mediaCoreI *svc = NULL;

  int	n = WASABI_API_SVC->service_getNumServices(WaSvc::MEDIACORE); 
		for(int h = 0; h < n && !svc; h++) { 
      svcf = WASABI_API_SVC->service_enumService(WaSvc::MEDIACORE, h); 
      svc = castService<svc_mediaCoreI>(svcf);

      if(svc) {
        cores.addItem(new Core(svc, (CoreToken)tokenindex, svcf->getServiceName()));
        //return (CoreToken)tokenindex;
        tokenindex++;
        return cores.getLast()->getToken();
      }
    }

    return NO_CORE_TOKEN;
}

int core_api::core_free(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c && core) {
    cores.removeItem(c);
    delete c;
    return 1;
  }
  return 0;
}

int core_api::core_setNextFile(CoreToken core, const char *playstring) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setNextFile(playstring, NULL);
    return 1;
  }
  return 0;
}

int core_api::core_getStatus(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c){
    return c->getPlaying();
  }
  return 0;
}

const char *core_api::core_getCurrent(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c){
    return c->getCurrent();
  }
  return NULL;
}

int core_api::core_getCurPlaybackNumber(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c){
    ItemSequencer * seq=c->getSequencer();
    if(seq) return seq->getCurrentPlaybackNumber();
  }
  return -1;
}

int core_api::core_getPosition(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c){
    return c->getPosition();
  }
  return -1;
}

int core_api::core_getWritePosition(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c){
    return c->getWritePosition();
  }
  return -1;
}

int core_api::core_setPosition(CoreToken core, int ms) {
  Core * c = tokenToCore(core);
  if(c){
    c->setPosition(ms);
    return 1;
  }
  return -1;
}

int core_api::core_getLength(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c){
    return c->getLength();
  }
  return -1;
}

int core_api::core_getPluginData(const char *playstring, const char *name, char *data, int data_len, int data_type) {

  //WTF?

  return -1;
}

unsigned int core_api::core_getVolume(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c) {
    return c->getVolume();
  }
  return 0;
}

void core_api::core_setVolume(CoreToken core, unsigned int vol) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setVolume(vol);
  }
}

int core_api::core_getPan(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c) {
    return c->getPan();
  }
  return -1;
}

void core_api::core_setPan(CoreToken core, int val) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setPan(val);
  } 
}

void core_api::core_addCallback(CoreToken core, CoreCallback *cb) {
  Core * c = tokenToCore(core);
  if(c){
    c->addCallback(cb);
  }
}

void core_api::core_delCallback(CoreToken core, CoreCallback *cb) {
 Core * c = tokenToCore(core);
  if(c){
    c->delCallback(cb);
  }
}

int core_api::core_getVisData(CoreToken core, void *dataptr, int sizedataptr) {
  Core * c = tokenToCore(core);
  if(c){
    return c->getVisData(dataptr, sizedataptr);
  }
  return 0;
}

int core_api::core_getLeftVuMeter(CoreToken core) {

  return 0;
}

int core_api::core_getRightVuMeter(CoreToken core) {
  
  return 0;
}

int core_api::core_registerSequencer(CoreToken core, ItemSequencer *seq) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setSequencer(seq);
    return 1;
  }
  return 0;
}

int core_api::core_deregisterSequencer(CoreToken core, ItemSequencer *seq) {
  Core * c = tokenToCore(core);
  if(c) {
    c->delSequencer(seq);
  }
  return 0;
}

void core_api::core_userButton(CoreToken core, int button) {
  Core * c = tokenToCore(core);
  if(c) {
    ItemSequencer *seq = c->getSequencer();
    switch(button) {
    case UserButton::PREV:
      if(seq) {
        seq->rewind();
        String current = seq->getNextPlayItem();
        c->setNextFile(current);
        c->abortCurrentSong();
        c->setCurrent(current);
      }
    break;
    case UserButton::PLAY: {
      if(seq) { // we have sequencer
        if(c->getPlaying()) c->stop();
        String current = seq->getNextPlayItem();
        c->setNextFile(current);
        c->start();
        c->setCurrent(current);
      } else { //only 1 file at a time being opened
        if(c->getPlaying()) c->stop();
        c->start();
      }
    }
    break;
    case UserButton::PAUSE: 
      c->pause(c->getPlaying() == -1?0 : 1);
    break;
    case UserButton::STOP: 
      c->stop();
    break;
    case UserButton::NEXT:
      if(seq){
        if(seq->forward()) {
        String current = seq->getNextPlayItem();
        c->setNextFile(current);
        c->abortCurrentSong();
        c->setCurrent(current);
        }
      }    
    break;
    }
  }
}

int core_api::core_getEqStatus(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c) {
    return c->getEQStatus();
  }
  return -1;
}

void core_api::core_setEqStatus(CoreToken core, int enable) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setEQStatus(enable);
  }
}

int core_api::core_getEqPreamp(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c) {
    return c->getEQPreamp();
  }
  return -1;
}

void core_api::core_setEqPreamp(CoreToken core, int pre) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setEQPreamp(pre);
  }
}

int core_api::core_getEqBand(CoreToken core, int band) {
  Core * c = tokenToCore(core);
  if(c) {
    return c->getEQBand(band);
  }
  return -1;
}

void core_api::core_setEqBand(CoreToken core, int band, int val) {
  Core * c = tokenToCore(core);
  if(c) {
    c->setEQBand(band,val);
  }
}

int core_api::core_getEqAuto(CoreToken core) {
  Core * c = tokenToCore(core);
  if(c) {
    return 0;
  }
  return -1;
}

void core_api::core_setEqAuto(CoreToken core, int enable) {
  Core * c = tokenToCore(core);
  if(c) {
    
  }
}

void core_api::core_setCustomMsg(CoreToken core, const char *text) {
  Core * c = tokenToCore(core);
  if(c) {
    //c->sendConvertersMsg(text, NULL);
    DebugString("CORE(%d): %s\n", core, text);
  }
}

void core_api::core_registerExtension(const char *extensions, const char *extension_name, const char *family) {
  PathParser parser(extensions, ";");
  int n = parser.getNumStrings();

  for(int i=0;i<n;i++) {
    SCAextension * ext= new SCAextension;
    ext->name=extension_name;
    ext->family=family;
    ext->ext=parser.enumString(i);
    SCAextensions.addItem(ext);
  }

  addfamily(family);
}

const char *core_api::core_getExtensionFamily(const char *extension) {
  String ext = extension;
  if(STRNCMP(ext, "*.", 2)) ext = String("*.") + ext;

  foreach(SCAextensions) {
    if(SCAextensions.getfor()->ext == extension) return SCAextensions.getfor()->family;
  } endfor;

  return NULL;
}

void core_api::core_unregisterExtension(const char *extensions) {
  String ext = extensions;
  SCAextension *toremove = NULL;

  if(STRNCMP(ext,"*.",2)) ext = String("*.")+ext;

  foreach(SCAextensions) {
    if(SCAextensions.getfor()->ext==ext) toremove = SCAextensions.getfor();
  } endfor;

  if(toremove){
    SCAextensions.removeItem(toremove);
    delete toremove;
  }
}

const char *core_api::core_getTitle() {
  Core * c = tokenToCore(0);

  if(c) {
    char buf[1024];
    int len = c->getTitle(buf, 1024);
    if(len > 0 && len < 1024) buf[len] = 0;

    return String(buf);
  }
  return NULL;
}

void core_api::addfamily(const char * name){
  foreach(SCAfamilies) {
    if(*(SCAfamilies.getfor()) == name) return;
  } endfor;

  SCAfamilies.addItem(new String(name));
}