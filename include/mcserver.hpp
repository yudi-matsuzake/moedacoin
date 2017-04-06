#ifndef __MCSERVER_H__
#define __MCSERVER_H__

#include <memory>

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>

#include "request.hpp"

/** Class to wrap a request and emit the signals appropriately
  *
  */
class MCServer : public QTcpServer{
	Q_OBJECT
public:
	MCServer(MCRequestDB* r, QObject* parent = nullptr);
	~MCServer();

	void timerStart();
signals:
	void response(MCRequestDB* request, MCResponseDB* response);

private slots:
	void onNewConnection();
	void onReadyRead();
	void onTimeout();

private:
	MCRequestDB* request;
	QTcpSocket* socket;
	static const unsigned int WAIT_RESPONSE_MS;
};

#endif
