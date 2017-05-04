#ifndef _NETWORK_H_
#define _NETWORK_H_

extern ip_addr_t hostIp;

void StartNetwork(char *_ssid,char *_pw);
void ResolveHost(char *hostName);
void HttpRequest(ip_addr_t hostIp, char *host, char *url);

#endif
