#include <UIPEthernet.h>
// The connection_data struct needs to be defined in an external file.
#include <UIPServer.h>
#include <UIPClient.h>

EthernetServer server = EthernetServer(8080);

void setup()
{
  Serial.begin(9600);

  uint8_t mac[6] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
  IPAddress myIP(192,168,0,203);

  Ethernet.begin(mac,myIP);

  server.begin();
  
  Serial.println("start tcp server");
  
}

void loop()
{
  size_t size;

  if (EthernetClient client = server.available())
    {
      if (client)
        {
          while((size = client.available()) > 0)
            {
              uint8_t* msg = (uint8_t*)malloc(size);
              size = client.read(msg,size);
              Serial.write(msg,size);
              client.write(msg,size);
              free(msg);
            }
        }
    }
}
