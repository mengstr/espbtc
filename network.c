#include "c_types.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"


#include "4bittube.h"
#include "network.h"

// Functions missing in SDK declarations :-(
int atoi(const char *nptr);
char *ets_strstr(const char *haystack, const char *needle);
void *ets_memcpy(void *dest, const void *src, size_t n);
int ets_sprintf(char *str, const char *format, ...)  __attribute__ ((format (printf, 2, 3)));
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));

//
struct ICACHE_FLASH_ATTR station_config wifiConf;
ip_addr_t hostIp;
struct espconn conn;
esp_tcp tcp;


#define MSG_DATAERR  0b00111001,0b01011100,0b00000100,0b01010100 // Coin


// Note! The sizes here must be adjusted to fit your data
#define MAXREPLYLEN 1024
#define MAXHOSTLEN 64
#define MAXURLLEN 64

char host[MAXHOSTLEN];
char url[MAXURLLEN];

//
//
//
void ICACHE_FLASH_ATTR GotDNS(
      const	char *name,	
      ip_addr_t	*ipaddr,
      void *arg)	{
//  struct espconn *pespconn=(struct espconn *)arg;
  if (ipaddr!=NULL) {
    hostIp.addr=ipaddr->addr;
    os_printf("Resolved IP is %d.%d.%d.%d\n",
      *((uint8 *)&ipaddr->addr),
      *((uint8 *)&ipaddr->addr+1),
      *((uint8 *)&ipaddr->addr+2),
      *((uint8 *)&ipaddr->addr+3));
  }

}

//
//
//
void ICACHE_FLASH_ATTR ResolveHost(char *hostName) {
  espconn_gethostbyname(
    &conn,
    hostName,
    &hostIp,
    GotDNS
  );
}

//
//
//
void ICACHE_FLASH_ATTR StartNetwork(char *_ssid,char *_pw) {
  strcpy((char *)wifiConf.ssid, _ssid);
  strcpy((char *)wifiConf.password, _pw);
  wifiConf.bssid_set=0;
  wifi_station_set_config_current(&wifiConf);
}

//
//
//
void ICACHE_FLASH_ATTR TcpDisconnected(void *arg) {
    os_printf("TcpDisconnected()\n");
}

//
//
//
void ICACHE_FLASH_ATTR TcpReceive(void *arg, char *data, uint16_t len) {
  char buf[MAXREPLYLEN];
  char *p;

  os_printf("TcpReceive()\n");
  // Find beginning of payload and copy it to the local buffer for processing
  p=strstr(data,"\r\n\r\n");
  p+=4;
  strncpy(buf,p,sizeof(buf));
  espconn_disconnect(&conn);

  // Extract value from JSON data and display it
  char sRate[20];
  uint32_t rate;
  int res;
  res=ExtractFromJson(buf,":bpi:USD:rate_float",sRate,sizeof(sRate));
  if (res==0) {
      Display(MSG_DATAERR);
  } else {
    rate=atoi(sRate);
    DisplayNumber(rate);
  }
}


//
//
//
void ICACHE_FLASH_ATTR TcpConnected(void *arg) {
  char buf[256];

  os_printf("TcpConnected()\n");

  ets_sprintf(buf,"GET %s HTTP/1.1\r\n" \
                  "Host: %s\r\n" \
                  "User-Agent: SmallRoomLabs\r\n" \
                  "Accept: */*\r\n" \
                  "\r\n\r\n", \
              url,host);
//   os_printf("[%s]\n",buf);

  espconn_regist_recvcb(&conn,TcpReceive);
  espconn_sent(&conn,(uint8_t *)buf,strlen(buf));
}

//
//
//
void ICACHE_FLASH_ATTR HttpRequest(ip_addr_t hostIp, char *_host, char *_url) {
  // Save the host/url for later usage in the connect callback function
  strcpy(host,_host);
  strcpy(url,_url);

  conn.type=ESPCONN_TCP;
  conn.state=ESPCONN_NONE;
  conn.proto.tcp=&tcp;
  conn.proto.tcp->local_port=espconn_port();
  conn.proto.tcp->remote_port=80;
  os_memcpy(conn.proto.tcp->remote_ip, &hostIp,4);

  DisplayDots(1);
  espconn_regist_connectcb(&conn, TcpConnected);
  espconn_regist_disconcb(&conn, TcpDisconnected);
  espconn_connect(&conn);
 }


/*


GET /v1/bpi/currentprice.json HTTP/1.1<crlf>
Host: api.coindesk.com<crlf>
User-Agent: curl/7.50.1<crlf>
Accept: * / *<crlf>
<crlf>
<crlf>



HTTP/1.1 200 OK..Date: Thu, 04 May 2017 06:34:54 GMT..Content-Type: applica
tion/javascript..Content-Length: 668..Set-Cookie: __cfduid=d845ff6c6639d25d
8714325da40065a8b1493879693; expires=Fri, 04-May-18 06:34:53 GMT; path=/; d
omain=.coindesk.com; HttpOnly..X-Powered-By: Bitcoin Love..Cache-Control: m
ax-age=15..Expires: Thu, 04 May 2017 06:35:07 UTC..X-BE: stinger..X-Proxy-C
ache: HIT..Access-Control-Allow-Origin: *..X-Cache-Status-A: HIT..Server: c
loudflare-nginx..CF-RAY: 35996fd616c1707a-SIN..Connection: keep-alive....{"
time":{"updated":"May 4, 2017 06:34:00 UTC","updatedISO":"2017-05-04T06:34:
00+00:00","updateduk":"May 4, 2017 at 07:34 BST"},"disclaimer":"This data w
as produced from the CoinDesk Bitcoin Price Index (USD). Non-USD currency d
ata converted using hourly conversion rate from openexchangerates.org","cha
rtName":"Bitcoin","bpi":{"USD":{"code":"USD","symbol":"&#36;","rate":"1,505
.8000","description":"United States Dollar","rate_float":1505.8},"GBP":{"co
de":"GBP","symbol":"&pound;","rate":"1,169.4419","description":"British Pou
nd Sterling","rate_float":1169.4419},"EUR":{"code":"EUR","symbol":"&euro;",
"rate":"1,382.2943","description":"Euro","rate_float":1382.2943}}}

*/
