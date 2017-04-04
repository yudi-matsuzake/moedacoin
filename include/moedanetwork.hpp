#ifndef MOEDANETWORK_HPP
#define MOEDANETWORK_HPP

#include <assert.h>

#include <QString>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonDocument>

//#include "request.hpp"

/**
 * @brief Class to abstract network operations and
 * handle connections.
 */
class MoedaNetwork : public QObject
{
Q_OBJECT
public:
	MoedaNetwork();

	/**
	 * @brief send `datagram` to multicast
	 * @param datagram data to be send
	 */
	void sendMulticast(QByteArray datagram);

	/**
	 * @brief send
	 * @param request
	 */
	//	void send(MCRequestDB& request);

signals:
	void datagramReceiveFromMulticast(QByteArray datagram);

private slots:
	void onReceiveDatagrams();

	void onResponseDB();

private:
	/*
	 * multicast
	 */
	static const QString MULTICAST_IP;
	static const short MULTICAST_PORT;
	static const short TTL;

	QUdpSocket multicastSocket;
	QUdpSocket multicastSendSocket;
	QHostAddress multicastGroupAddress;

	void initMulticast();
};

#endif // MOEDANETWORK_HPP
