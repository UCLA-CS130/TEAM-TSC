#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

class ServerInterface {
public:
	virtual ~ServerInterface() {};

	virtual void run() = 0;

	virtual void do_accept() = 0;

};

#endif // CONNECTION_INTERFACE_H