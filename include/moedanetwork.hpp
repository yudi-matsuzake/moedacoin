#ifndef MOEDANETWORK_HPP
#define MOEDANETWORK_HPP

#include <memory>
#include <assert.h>

#include <QString>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSignalMapper>
#include <QtNetwork>

#include "request.hpp"
#include "mcserver.hpp"

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
	void send(MCRequestDB* request);

	/** Gets the ip of the machine.
	  *
	  * @return the ip of the network interface
	  */
	QHostAddress getMachineIP (void);

signals:
	void datagramReceiveFromMulticast(QByteArray datagram);

	/**
	 * @brief This signal is emitted when a response to db_request
	 * @param request pointer to request this must be freed
	 * @param response pointer to response; if the timeout was reached
	 * 	  this will point to null
	 */
	void dbResponse(MCRequestDB* request, MCResponseDB* response);

private slots:
	void onReceiveDatagrams();

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

	/*
	 *
	 */
	std::unique_ptr<QSignalMapper> signalMapper;

	void initMulticast();
};

#endif // MOEDANETWORK_HPP
