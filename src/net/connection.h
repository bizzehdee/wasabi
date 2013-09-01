#ifndef _CONNECTION_H
#define _CONNECTION_H

// simple class to break up a TCP stream into packets. dunno whether
// to merge it with jnetlib or wasabi --BU

#include <bfc/std.h>

class JNL_Connection;
class Client;

class Connection {
public:
  Connection(const char *server_name, int port);
  Connection(JNL_Connection *conn);
  ~Connection();

  int connected();

  void disconnect(int quick=FALSE);

  void run();	// call in a looop

  int recv_message(void *msg, int maxlen);
  int send_message(void *msg, int len);

  template <class T> int send(const T &msg) {
    return send_message((void*)&msg, sizeof(T));
  }

private:
  JNL_Connection *conn;
};

#endif
