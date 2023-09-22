#include <unistd.h>
#include "server/webServer.hpp"

int main()
{
  WebServer server
  (
      9999,3,60000,false,
    3306,"root","123456","webserver",
    12,6,true,1,1024
    );
  server.start();
}
