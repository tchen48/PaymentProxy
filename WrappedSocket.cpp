#include "controlChar.h"
#include "stdafx.h"
# include "WrappedSocket.h"

#include <iostream>
#include <ws2tcpip.h>
#include <wincrypt.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
# include <openssl/conf.h>
# include <openssl/err.h>


	class WrappedSocket {
	private:
		bool _isConnected = false;
		SOCKET _socket;
		SSL * _ssl;
		long _timeout;
		void _initOpenSslLib(void){
			(void)SSL_library_init();
			SSL_load_error_strings();
			//TODO: need to consider multithreading in here
			OPENSSL_config(NULL);
		}

		int _createSocketAndConnect(char* host, char* port){
			WSADATA wsadata;
			if (WSAStartup(0x0202, &wsadata) || wsadata.wVersion != 0x0202){
				WSACleanup();
				return WSAGetLastError();
			}

			//GET IP ACCORDING TO HOSTNAME, MAY GET MULTIPLE IP BY 1 HOST NAME?
			struct addrinfo * addrInfo = NULL, *ptr = NULL, hints;
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			if (getaddrinfo(host, port, &hints, &addrInfo) != 0) {
				WSACleanup();
				return WSAGetLastError();
			}

			//ATTEMPT TO CONNECT AN ADDRESS
			for (ptr = addrInfo; ptr != NULL; ptr = ptr->ai_next) {
				_socket = socket(ptr->ai_family, ptr->ai_socktype,
					ptr->ai_protocol);
				if (_socket == INVALID_SOCKET) {
					WSACleanup();
					return WSAGetLastError();
				}
				if (connect(_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
					closesocket(_socket);
					_socket = INVALID_SOCKET;
					continue;
				}
				_isConnected = true;
				break;
			}
			freeaddrinfo(addrInfo);
			return _socket== INVALID_SOCKET? WSAGetLastError():1;
		}
	public:
		int ConnectWithSSL(char* host, char* port, int type)
		{
			//ONE INSTANCE CAN ONLY HAVE ONE CONNECTION (HOW ABOUT MULTITHREAD?)
			if (_isConnected)
				return -1;

			//INITIALIZE SSL CONTEXT USING TYPE
			SSL_CTX* ctx = NULL;
			_initOpenSslLib();
			switch (type) {
			case TLS_SSL:
					ctx = SSL_CTX_new(SSLv23_method());
			case TLSv1_2:
				ctx = SSL_CTX_new(TLSv1_2_method());
			case TLSv1_1:
				ctx = SSL_CTX_new(TLSv1_1_method());
			case SSLv3:
				ctx = SSL_CTX_new(SSLv3_method());
			case SSLv2:
				ctx = SSL_CTX_new(SSLv23_method());
			}
			if (ctx == NULL)
				return INVALID_SSL_TYPE;
			//MAY DEFINE CERTIFICATE HERE (ONLY WHEN PEER VERIFY OUR CERTIFICATE)
			//MAY DEFINE VERIFICATION OPTION HERE (DECREASE CHANCE OF SUCCESSFUL CONNECTION)
			_ssl = SSL_new(ctx);

			_createSocketAndConnect(host, port);
			if (_socket == INVALID_SOCKET || _socket == NULL)
				return -1;

			//USE BASIC IO ABSTRATION TO SET SSL CONNECTION
			auto sbio = BIO_new(BIO_s_socket());
			BIO_set_fd(sbio, _socket, BIO_NOCLOSE);
			SSL_set_bio(_ssl, sbio, sbio);
			auto err = SSL_connect(_ssl);
			ERR_print_errors_fp(stdout);
		}

		buffer_st Receive()
		{
			buffer_st buffInfo;
			char buffer[3500];
			//MAY NEED TO IMPLEMENTING LOCKING HERE BUT IF ONLY ONE INSTANCE MAY NOT NEED TO
			if (!_isConnected)
				return buffInfo;
			auto length = SSL_read(_ssl, buffer, sizeof(buffer));
			if (length <= 0)
				return buffInfo;
			buffInfo.buff = buffer;
			buffInfo.size = length;

		}


		int Send(buffer_st buffInfo)
		{
			if (!_isConnected)
				return -1;
			auto length = SSL_write(_ssl, buffInfo.buff, buffInfo.size);
			return length;

		}

	};