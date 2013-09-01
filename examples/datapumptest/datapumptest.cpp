/*
g++ -D_DEBUG -DLINUX -I../../wasabi/src -I./ `find ../../wasabi/src/api/datapump -name '*.cpp' -printf '%p '` ../../wasabi/src/bfc/guid.cpp ../../wasabi/src/bfc/assert.cpp ../../wasabi/src/bfc/std_mem.cpp ../../wasabi/src/bfc/std_string.cpp ../../wasabi/src/bfc/std_file.cpp ../../wasabi/src/bfc/std_cpu.cpp ../../wasabi/src/bfc/string/string.cpp ../../wasabi/src/bfc/platform/platform.cpp ../../wasabi/src/bfc/platform/posix/posix.cpp ../../wasabi/src/bfc/file/readdir.cpp ../../wasabi/src/bfc/ptrlist.cpp ../../wasabi/src/bfc/foreach.cpp ../../wasabi/src/bfc/std.cpp ../../wasabi/src/parse/pathparse.cpp ../../wasabi/src/bfc/memblock.cpp ../../wasabi/src/bfc/thread.cpp ../../wasabi/src/bfc/critsec.cpp /usr/lib/libuuid.so /usr/lib/libpthread.so -o datapumptest datapumptest.cpp
*/
#include <stdio.h>
#include <string.h>

#include <api/datapump/api_pumpi.h>
static api_pumpI pump_api;

#include <api/datapump/pumpobj.h>
#include <api/datapump/pumphandler.h>
#include <bfc/circbuf.h>
#include <bfc/critsec.h>
#include <bfc/guid.h>

// ===============================================================================================================
// Sample dataline:
// ===============================================================================================================

/*
   +------------------------+
   | DSP Input Data Source  |
   |          O             |
   +----------+-------------+
              |
              |
              V
   +----------+-------------+
   |          V             |  <--  DSP Process Input Data Puller
   |         DSP            |
   |          V             |  <--  DSP Process Output Data Pusher
   +----------+-------------+
              |
              |
              V
   +----------+-------------+
   |          O             |
   | DSP Output Data Sink   |   
   +------------------------+

*/

static GUID DSPInputGUID = { 0x63cd46a4, 0x1744, 0x0d42, { 0x90, 0x0c, 0xdd, 0x88, 0xb6, 0x4d, 0xc5, 0x1b } };
static GUID DSPProcessGUID = { 0x3eec25fe, 0xc480, 0xb548, { 0x83, 0x8f, 0xb7, 0xe5, 0x38, 0x11, 0xfa, 0x37 } };
static GUID DSPOutputGUID = { 0x6f40757d, 0x599a, 0x714a, { 0x9f, 0x6f, 0x8f, 0xa1, 0x36, 0x3f, 0xc3, 0xaf } };

class DSPInput : public DataSource {
public:
  DSPInput() : DataSource(DSPInputGUID, "DSP Input Data Source"), num(0) {};
  
  virtual int getData(GUID request_from, void *data, unsigned long sz, unsigned long *actual_sz) {
    *actual_sz = 0;
    if(++num % 50) return 0;
    if(request_from != DSPProcessGUID) return 0;
    char *c = (char *)data;
    sprintf(c, "<some data from DSPInput:%d>",num);
    *actual_sz = strlen(c) * sizeof(char);
    return 1;
  }
  
private:
  int num;
};

class DSPProcess : public Thread, protected DataPuller, protected DataPusher {
public:
  DSPProcess() : DataPuller(DSPInputGUID, DSPProcessGUID, "DSP Process Input Data Puller"), 
    DataPusher(DSPOutputGUID, DSPProcessGUID, "DSP Process Output Data Pusher") {}
  
  int threadProc() {
    char *data = (char *)malloc(1024 * sizeof(char));
    unsigned long sz = 0;
    while(!getKillSwitch()) {
      if (!pullData(data, 1024 * sizeof(char), &sz) || sz == 0) {
        Std::usleep(10 /*ms*/);
        continue;
      }
      //process the data:
      strcat(data, ":processed by DSPProcess:");
      pushData(data, (strlen(data)+1) * sizeof(char));
    }
   free(data);
   printf("DSPProcess: got killswitch\n"); fflush(stdout);
   }
};

class DSPOutput : public DataSink {
public:
  DSPOutput() : DataSink(DSPOutputGUID, "DSP Output Data Sink") {};

  virtual int onData(GUID from, void *data, unsigned long sz) { 
    printf("DSPOutput: got data: %s\n", data); fflush(stdout);
    return 1; 
  }

};

// ===============================================================================================================
// Sample dataline 2:
// ===============================================================================================================
// this one is slightly more tricky.
/*
   +------------------------+
   | DSP2 Input DataPusher  |
   |          V             |
   +----------+-------------+
              |
              |
              V
   +----------+-------------+
   |          O             |  <--  DSP Process Input Data Sink
   |         DSP2           |
   |          O             |  <--  DSP Process Output Data Source
   +----------+-------------+
              |
              |
              V
   +----------+-------------+
   |          V             |
   | DSP2Output DataPuller  |   
   +------------------------+

*/
const GUID DSP2InputGUID = { 0x12f85d34, 0x1c18, 0x1e4f, { 0x9f, 0x18, 0xbe, 0xbb, 0xd8, 0xb2, 0xc0, 0x7a } };
const GUID DSP2ProcessGUID = { 0x8dc819a0, 0x52ce, 0x1045, { 0xb2, 0xa9, 0x44, 0x96, 0x50, 0x55, 0xae, 0xd0 } };
const GUID DSP2OutputGUID = { 0xe7bf79c8, 0x1aa9, 0x2f40, { 0xa9, 0x68, 0x7c, 0xa3, 0x14, 0xe4, 0x67, 0x88 } };

class DSP2Input : public Thread, protected DataPusher {
public:  
  DSP2Input() : DataPusher(DSP2ProcessGUID, DSP2InputGUID, "DSP2 Input Data Pusher") {}
  
  virtual int threadProc() {
    char *data = (char *)malloc(128 * sizeof(char));
    int n = 0;
    while(!getKillSwitch()) {
      if(++n % 50) {
        Std::usleep(10/*ms*/);
      } else {
        sprintf(data,"<some data from DSP2Input:%d>",n);
        pushData(data, (strlen(data)+1) * sizeof(char));
        n++;
      }
    }
  }
  
};

class DSP2Process : public DataSink, public DataSource {
public:
  DSP2Process() : DataSink(DSP2ProcessGUID, "DSP2 Process Data Sink"),
      DataSource(DSP2ProcessGUID, "DSP2 Process Data Source") {
    buf.open(1024 * sizeof(char));
  }
  virtual ~DSP2Process() {
    buf.close();
  }
  
  // in
  virtual int onData(GUID from, void *data, unsigned long sz) {
    if(from != DSP2InputGUID) return 0;
    INCRITICALSECTION(cs);
    buf.putBytes((char *)data, sz - 1 );
    String cat(":Processsed by DSP2Process:");
    buf.putBytes(cat.vs(), cat.len()+1);
    return 1;
  }
  
  // out
  virtual int getData(GUID request_from, void *data, unsigned long sz, unsigned long *actual_sz) {
    if(request_from != DSP2OutputGUID) return 0;
    INCRITICALSECTION(cs);
    *actual_sz = buf.getBytes((char *)data, sz);
    return 1;
  }

private:
  CriticalSection cs;
  CircBuf buf;
};

class DSP2Output : public Thread, protected DataPuller {
public:
  DSP2Output() : DataPuller(DSP2ProcessGUID, DSP2OutputGUID, "DSP2 Output Data Puller") {}
  
  virtual int threadProc() {
    char *data = (char *)malloc(128 * sizeof(char));
    int n = 0;
    unsigned long sz = 0;
    while(!getKillSwitch()) {
      if ( !(++n % 50)
          && pullData(data, 128 * sizeof(char), &sz)) {
        for(int i = 0; i < sz; i++) {
          printf("DSP2Output: got data: %s\n",data+i);
          i += strlen(data+i);
        }
      } else {
        Std::usleep(10/*ms*/);
      }
    }
  }
  
};

// ===============================================================================================================

// ===============================================================================================================
// Sample data broadcast:
// ===============================================================================================================
/*
                               +-----------------------+
                               |    BCast DataPusher   |
                               |           V           |
                               +-----------+-----------+
                                           |
                                           |
                                           V
                                     ------+------
                                    /      O      \
                                   /  Broadcaster  \  <-- DataBroadcast forwards incoming "from" GUID,
                                  /       VVV       \       so data looks like it's coming from the Pusher.
                                 /---------+---------\
                ___________________________|___________________________
               |                           |                           |
               V                           V                           V
   +-----------+-----------+   +-----------+-----------+   +-----------+-----------+
   |           O           |   |           O           |   |          O            |   <-- (DataSinks)
   |  BCast DataListener1  |   |  BCast DataListener2  |   |  BCast DataListener3  |
   +-----------------------+   +-----------------------+   +-----------------------+

*/

#include <api/datapump/broadcast.h>
//{50b4acb0-8ddb-4581-a263-f9112e3975f7}
static const GUID MyBCastPusherGUID =
{ 0x50b4acb0, 0x8ddb, 0x4581, { 0xa2, 0x63, 0xf9, 0x11, 0x2e, 0x39, 0x75, 0xf7 } };
//{85a0773a-fdf9-4b2e-8f08-16ae3ddde118}
static const GUID MyBroadcasterGUID = 
{ 0x85a0773a, 0xfdf9, 0x4b2e, { 0x8f, 0x08, 0x16, 0xae, 0x3d, 0xdd, 0xe1, 0x18 } };

class MyBCastPusher : public Thread, protected DataPusher {
  public:  
    MyBCastPusher() : DataPusher(MyBroadcasterGUID, MyBCastPusherGUID, "Broadcast Pusher") {}
  
    virtual int threadProc() {
      char *data = (char *)malloc(128 * sizeof(char));
      int n = 0;
      while(!getKillSwitch()) {
        if(++n % 50) {
          Std::usleep(10/*ms*/);
        } else {
          sprintf(data,"<some data from DSP2Input:%d>",n);
          pushData(data, (strlen(data)+1) * sizeof(char));
          n++;
        }
      }
    }
  
};

class MyBCastListener : public DataBroadcastListener {
public:
  MyBCastListener(GUID listento, int id) : DataBroadcastListener(listento, Guid(), StringPrintf("MyBCastListener#%d",id)) {};
  virtual ~MyBCastListener() {};
  
  virtual int onData(GUID from, void *data, unsigned long sz) {
    if(from == MyBCastPusherGUID) {
      printf("%s: got data: %s\n", getName(), data); fflush(stdout);
      return 1; 
    }
  }
};

// ===============================================================================================================


int main() {
  pumpApi = &pump_api;
  
  printf("Starting: Local Pipe Handler\n");    fflush(stdout);
  WASABI_API_DATAPUMP->addPumpHandler(new LocalPumpHandler());
  if(WASABI_API_DATAPUMP->getPumpHandler(PUMP_LOCAL)) {
    printf("Handler added.\n");    fflush(stdout);
  } else {
    printf("!!ERROR!!: Handler not found. Quitting...\n"); fflush(stdout);
    return 0;
  }
  
  {
    printf("\n===== Test 1: sample DSP dataline. (active middle) =====\n"); fflush(stdout);
    DSPInput in;
    DSPProcess dsp;
    DSPOutput out;
    const int sec = 5;
    printf("Letting DSPProcess run for %d seconds...\n\n",sec); fflush(stdout);
    dsp.start();
    sleep(sec);
    dsp.setKillSwitch();
    dsp.end();
    printf("\nStopped DSPProcess.\n");
    
  }
  printf("\n**End of Test 1.\n"); fflush(stdout);
  
  {
    printf("\n===== Test 2: sample DSP dataline. (active ends) =====\n"); fflush(stdout);
    DSP2Input in;
    DSP2Process dsp;
    DSP2Output out;
    const int sec = 5;
    printf("Letting DSP2Input and DSP2Output run for %d seconds...\n\n",sec); fflush(stdout);
    in.start();
    out.start();
    sleep(sec);
    in.setKillSwitch();
    out.setKillSwitch();
    in.end();
    printf("\nStopped DSP2Input.\n"); fflush(stdout);
    out.end();
    printf("Stopped DSP2Output.\n"); fflush(stdout);
  }
  printf("\n**End of Test 2.\n"); fflush(stdout);
  
  {
    printf("\n===== Test 3: sample data broadcast. =====\n"); fflush(stdout);
    MyBCastPusher push;
    DataBroadcast bcast(MyBroadcasterGUID, "my broadcaster");
    MyBCastListener l1(MyBroadcasterGUID, 1);
    MyBCastListener l2(MyBroadcasterGUID, 2);
    MyBCastListener l3(MyBroadcasterGUID, 3);
    const int sec = 5;
    printf("Letting MyBCastPusher run for %d seconds...\n\n",sec); fflush(stdout);
    push.start();
    sleep(sec/2);
    printf(" **Disconnecting MyBCastListener#2\n");
    l2.disconnect();
    sleep(sec/2);
    push.setKillSwitch();
    push.end();
    printf("\nStopped MyBCastPusher.\n"); fflush(stdout);   
  }
  printf("\n**End of Test 3.\n"); fflush(stdout);

  printf("\nGoodbye.\n"); fflush(stdout);
  return 0;
}
