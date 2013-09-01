
#include "connection.h"

#include <net/jnetlib/connection.h>

#ifdef WIN32
#include "winsock2.h"
#endif

Connection::Connection(const char *server_name, int port) {
DebugString("connecting to %s:%d", server_name, port);
  conn = new JNL_Connection;
  conn->connect(const_cast<char*>(server_name), port);
  stdtimevalms now = Std::getTimeStampMS();
  while (Std::getTimeStampMS() - now < 30) {
    conn->run();
    int state = conn->get_state();
    int quit = 0;
    switch (state) {
      case JNL_Connection::STATE_ERROR:
      case JNL_Connection::STATE_CONNECTED: quit = 1; break;
      case JNL_Connection::STATE_RESOLVING:
      case JNL_Connection::STATE_CONNECTING:
      break;
    }
    if (quit) break;
    Std::usleep(1000);
DebugString("got state %d", conn->get_state());
  }
DebugString("done %d", conn->get_state());
}

Connection::Connection(JNL_Connection *_conn)
  : conn(_conn) {
}

Connection::~Connection() {
  disconnect();
}

int Connection::connected() {
  if (conn == NULL) return 0;
  return (conn->get_state() == JNL_Connection::STATE_CONNECTED);
}

void Connection::disconnect(int quick) {
  if (conn != NULL) {
    conn->close(quick);
    delete conn; conn = NULL;
  }
}

void Connection::run() {
  if (conn != NULL) conn->run();
}

int Connection::recv_message(void *msg, int maxlen) {
  if (conn == NULL) return 0;
  int bytes_avail = conn->recv_bytes_available();
  if (bytes_avail < sizeof(int)) return 0;
  unsigned long nlen=0;
  conn->peek_bytes(&nlen, sizeof(nlen));
  int msglen = ntohl(nlen);
  if (bytes_avail < sizeof(msglen)+msglen) return 0; 	// not all here jet
  conn->recv_bytes(NULL, 4);	// eat the length field
  if (maxlen < msglen) {
    ASSERTPR(0, "FIX");
  } else {
    conn->recv_bytes(msg, msglen);
  }
  return 1;
}

int Connection::send_message(void *msg, int len) {
  if (conn == NULL) return 0;
  ASSERT(msg != NULL);
  ASSERT(len >= 0);
  if (len == 0) return 1;
//  unsigned char *charp = (unsigned char *)msg;
//DebugString("sendmessage %d", (unsigned int)*charp);
  unsigned long nlen = htonl(len);
  conn->send_bytes(&nlen, sizeof(nlen));
  conn->send_bytes(msg, len);
//CUT ASSERT(len <= MAX_MESSAGE_SIZE);
  return 1;
}
