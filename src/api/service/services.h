#ifndef _SERVICES_H
#define _SERVICES_H

#include <bfc/std_mkncc.h> // for MKnCC()

// This is for old service types that you might revive from an old project and want old compiles to work. 
// Don't use it unless you need to!
#define SVCTGUID_4CC(x) { x, 0xe2a5, 0x7845, { 0x9d, 0x03, 0x35, 0xb8, 0xd0, 0xc1, 0xba, 0xc7 } }
#define SVCTGUID_MK4CC(a,b,c,d) SVCTGUID_4CC( MK4CC(a,b,c,d) )

// if you have a service that is unique to a component (ie, there can be only one implementation of it 
// [not just that you could only *call* one of it at a time]), make it type UNIQUE and register it by GUID,
// otherwise, either use one of the global types (the types used by public wasabi apis) or use your
// own type GUID.

// WaI> I don't really like this big namespace. I think it would be better for service types to declare
//      their service type guid each in their own .h, though i see the advantage of having a single
//      namespace. Maybe this is ok if we keep it down to very general/universal service types.
//       e.g., no PLAYLISTREADER here ;)
// lone> these are the ones that any public wasabi api might use, it's meant to be a minimal set.

namespace WaSvc {
  const GUID ACCESSIBILITY=SVCTGUID_MK4CC('a','c','c','s');
  const GUID ACCESSIBILITYROLESERVER=SVCTGUID_MK4CC('a','c','r','s');
  const GUID COLLECTION=SVCTGUID_MK4CC('c','l','t','n');
  const GUID CONTEXTCMD=SVCTGUID_MK4CC('c','c','m','d');
  const GUID DB=SVCTGUID_MK4CC('d','b','s','e');
  const GUID EVALUATOR=SVCTGUID_MK4CC('e','v','a','l');
  const GUID FILESELECTOR=SVCTGUID_MK4CC('f','s','e','l');
  const GUID ITEMMANAGER=SVCTGUID_MK4CC('i','t','m','g');
  const GUID METADATA=SVCTGUID_MK4CC('m','e','t','a');
  const GUID MINIBROWSER=SVCTGUID_MK4CC('b','r','w','s');
  const GUID OBJECTDIR=SVCTGUID_MK4CC('o','b','j','d');
  const GUID PLAYLISTREADER=SVCTGUID_MK4CC('p','l','r','d');
  const GUID PLAYLISTWRITER=SVCTGUID_MK4CC('p','l','w','t');
  const GUID STORAGEVOLENUM=SVCTGUID_MK4CC('s','v','e','n');
  const GUID TOOLTIPSRENDERER=SVCTGUID_MK4CC('t','t','i','p');
  const GUID WINDOWCREATE=SVCTGUID_MK4CC('w','n','d','c');
  const GUID NONE=SVCTGUID_MK4CC('n','o','n','e');
  const GUID UNIQUE=SVCTGUID_MK4CC('u','n','i','q');                     // for unique services, enumed by GUID
  const GUID IMAGELOADER=SVCTGUID_MK4CC('i','m','g','l');                // image loader		svc_imgload.h
  const GUID FONTRENDER=SVCTGUID_MK4CC('f','o','n','t');                 // font renderer (bitmap/truetype/freetype)
  const GUID STRINGCONVERTER=SVCTGUID_MK4CC('u','t','f','8');            // unicode string conversion
  const GUID XMLPROVIDER=SVCTGUID_MK4CC('x','m','l','p');                // xml provider
  const GUID REDIRECT=SVCTGUID_MK4CC('r','e','d','r');                   // filename redirect
  const GUID IMAGEGENERATOR=SVCTGUID_MK4CC('i','m','g','n');             // image generator	svc_imggen.h
  const GUID ACTION=SVCTGUID_MK4CC('\0','a','c','t');                    // custom actions (ie: for buttons)
  const GUID SRCCLASSFACTORY=SVCTGUID_MK4CC('c','l','f','a');            // source code class factory
  const GUID FILEREADER=SVCTGUID_MK4CC('f','s','r','d');                 // file system reader (disk, zip, http)
  const GUID SKINFILTER=SVCTGUID_MK4CC('f','l','t','r');                 // bitmap/colorref skin filter
#ifdef LEGACY_XUI
  const GUID XUIOBJECT=SVCTGUID_MK4CC('x','u','i','o');                  // nullsoft xml gui objects
#else
  const GUID XUIOBJECT=SVCTGUID_MK4CC('x','u','i','2');                  // free xml gui objects
#endif
  const GUID DROPTARGET=SVCTGUID_MK4CC('d','r','o','p');	               // drop targets
  const GUID SCRIPTOBJECT=SVCTGUID_MK4CC('m','a','k','i');               // third party script object
  // NOTE: before adding something, read notes above about SVCTGUID_MK4CC
};

/* Old Stuff ================================================================================================ */
/*   Revive above as necessary                                                                                      */


// lower-case service names are reserved by Nullsoft for future use
// upper-case service names are for 3rd parties to extend the service system

namespace LegacyWaSvc {
  enum {
    NONE=MK4CC('n','o','n','e'),
    UNIQUE=MK4CC('u','n','i','q'),                  // for unique services, enumed by GUID
    CONTEXTCMD=MK4CC('c','c','m','d'),              // context menu command	svc_contextCmd.h
    DEVICE=MK3CC('d','e','v'),                   // portable device	svc_device.h
    FILEREADER=MK4CC('f','s','r','d'),	            // file system reader (disk, zip, http)
    FILESELECTOR=MK4CC('f','s','e','l'),            // file selector	svc_filesel.h
    STORAGEVOLENUM=MK4CC('f','s','e','n'),          // storage volume enumerator.
    IMAGEGENERATOR=MK4CC('i','m','g','n'),          // image generator	svc_imggen.h
    IMAGELOADER=MK4CC('i','m','g','l'),	            // image loader		svc_imgload.h
    IMAGEWRITER=MK4CC('i','m','g','w'),		// image writer
    ITEMMANAGER=MK4CC('i','m','g','r'),	            // item manager		svc_itemmgr.h
    PLAYLISTREADER=MK4CC('p','l','r','d'),          // playlist reader
    PLAYLISTWRITER=MK4CC('p','l','w','r'),          // playlist writer
    MEDIACONVERTER=MK4CC('c','o','n','v'),          // media converter
    MEDIACORE=MK4CC('c','o','r','e'),               // media core
    MEDIARECORDER=MK4CC('m','r','e','c'),           // media recorder
    SCRIPTOBJECT=MK4CC('m','a','k','i'),            // third party script object
//    TRANSLATOR=MK4CC('x','l','a','t'),            // text translator
    WINDOWCREATE=MK4CC('w','n','d','c'),            // window creator
    DB=MK2CC('d','b'),                        // database
    SKINFILTER=MK4CC('f','l','t','r'),              // bitmap/colorref skin filter
    METADATA=MK4CC('m','t','d','t'),                // play item meta data
    METATAG=MK4CC('m','t','t','g'),                 // metadata tagging of play items
    EVALUATOR=MK4CC('e','v','a','l'),               // evaluate a string
    MINIBROWSER=MK2CC('m','b'),               // minibrowser
    TOOLTIPSRENDERER=MK4CC('t','t','i','p'),        // tooltips renderer
    XUIOBJECT=MK4CC('x','u','i','o'),               // xml gui objects
    STRINGCONVERTER=MK4CC('u','t','f','8'),         // unicode string conversion
    ACTIONN=MK3CC('a','c','t'),                   // custom actions (ie: for buttons)
    COREADMIN=MK4CC('c','a','d','m'),               // core administrator
    DROPTARGET=MK4CC('d','r','o','p'),	            // drop targets
    OBJECTDIR=MK4CC('o','b','j','d'),               // object directory
    TEXTFEED=MK4CC('t','x','t','f'),                // text feed, to send text to various XUI objects (ie: <Text> by using display="textfeedid"
    ACCESSIBILITY=MK4CC('a','c','c','s'),           // accessibility service
    ACCESSIBILITYROLESERVER=MK4CC('r','o','l','e'), // accessibility roleServer services
    EXPORTER=MK3CC('e','x','p'),                 // exporter
    COLLECTION=MK4CC('c','l','c','t'),              // named xml overridable collection
    REDIRECT=MK4CC('r','e','d','r'),                // filename redirect
    FONTRENDER=MK4CC('f','o','n','t'),              // font renderer (bitmap/truetype/freetype)
    SRCCLASSFACTORY=MK4CC('c','l','f','a'),         // source code class factory
    SRCEDITOR=MK4CC('s','e','d','t'),               // source code editor
  };
};

#endif
