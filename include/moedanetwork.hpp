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

#include "mcrequest.hpp"
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
	 * @brief Send a MCRequest
	 * @param class inherited from MCRequest
	 */
	void send(MCRequestDB* request);
	void send(MCResponseDB* request);
	void send(MCRequestMiner* request);
	void send(MCResponseMiner* request);
	void send(MCRequestUpdate* request);

	/** Gets the ip of the machine.
	  *
	  * @return the ip of the network interface
	  */
	QHostAddress getMachineIP (void);

signals:

	/**
	 * @brief This signal is emitted when a response to db_request
	 * @param request pointer to request this must be freed
	 * @param response pointer to response; if the timeout was reached
	 * 	  this will point to null
	 */
	void responseDB(MCRequestDB* request, MCResponseDB* response);

	/** This signal is emitted when a requestDB was received.
	  * TODO: A more datailed description of receiveRequestDB.
	  *
	  * @param requestDB	a reference to the requestDB
	  */
	void requestDB (MCRequestDB* requestDB);

	/** This signal is emitted when a ResponseMIner is received
	  *
	  * @param	responseMiner is the reference to the responseMiner
	  * 		that came from the network
	  */
	void responseMiner(MCResponseMiner* responseMiner);

	/** This signal is emmited when miner request is received
	  *
	  * @param request
	  */
	void requestMiner(MCRequestMiner* request);

	/**
	 * @brief This signal is emitted when a requestUpdate was received
	 * @param request a reference to requestUpdate
	 */
	void requestUpdate(MCRequestUpdate* request);

private slots:
	void onReceiveDatagrams();
	void onResponseDbConnect();
	void onResponseMiner();

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

	std::map<QTcpSocket*, MCResponseDB*> responseDbMap;
	std::map<QUdpSocket*, MCRequestMiner*> requestMinerMap;

	void initMulticast();
};

#endif // MOEDANETWORK_HPP
