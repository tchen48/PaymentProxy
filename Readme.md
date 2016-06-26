#Payment Proxy Library

the target of this project is to provide an open source payment interface for Commercial payment industry or individual merchants. The final version will be cross platform, but the current virson is developed under Windows system. This project is using openssl to provide secure tcp connection

##one use case of vital proxy

WrappedSocket s;
s.ConnectWithSSL("bryce.ascertified.com", "15004", 0);


auto buff = s.Receive();
if (buff.buff[0] != ENQ) {
	s.CloseConnection();
	return 1;
}

ControlCharOperator op;
string str = "---the request message string here---";
char *data = new char[str.length() + 1];
strcpy_s(data, str.length() + 1, str.c_str());
char* etx = new char[1];
etx[0] = ETX;
char* stx = new char[1];
stx[0] = STX;
char* data_etx = op.combine(data, str.length(), etx, 1);
char* lrc = op.ComputeLRC(data_etx, str.length()+1);
char* data_etx_lrc = op.combine(data_etx, str.length() + 1, lrc, 1);
char* stx_data_ext_lrc = op.combine(stx, 1, data_etx_lrc, str.length() + 2);
size_t size= str.length() + 3;
for (int i = 0; i < size; i++) {
	op.AddParity(stx_data_ext_lrc[i]);
}
buff.buff = stx_data_ext_lrc;
buff.size = size;
s.Send(buff);

buff = s.Receive();
if (buff.buff[0] != ACK) {
s.CloseConnection();
return 1;
}

buff = s.Receive();
for (int i = 1; i < buff.size-2; i++) {
op.RemoveParity(buff.buff[i]);
}
s.CloseConnection();
