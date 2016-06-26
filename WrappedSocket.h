#pragma once
#ifndef WRAPPED_SOCKET_H
#define WRAPPED_SOCKET_H

#define TLS_SSL 0	
#define TLSv1_2 1
#define TLSv1_1 2
#define SSLv2 3
#define SSLv3 4

struct buffer_st {
	char * buff;
	unsigned int size;

};

#define INVALID_SSL_TYPE 1000

#endif // !WRAPPED_SOCKET_H
