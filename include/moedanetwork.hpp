#ifndef MOEDANETWORK_HPP
#define MOEDANETWORK_HPP

#include <QString>
#include <QUdpSocket>
#include <QHostAddress>
#include <QJsonObject>

/**
 * @brief Class to abstract network operations and
 * handle connections.
 */
class MoedaNetwork : public QObject
{
Q_OBJECT
public:
	/*
	 * inner classes
	 */
	class Peer;
	class Request;
	class Response;
	class RequestDB;
	class ResponseDB;
	class RequestMiner;
	class ResponseMiner;
	class RequestUpdate;

	MoedaNetwork();

	/**
	 * @brief send `datagram` to multicast
	 * @param datagram data to be send
	 */
	void sendMulticast(QByteArray datagram);

	/**
	 * @brief Represents a peer connected to the host.
	 */
	class Peer{
	public:
		Peer();
		Peer(QHostAddress address,
			QString name,
			qint16 port);

		~Peer();

		QHostAddress getPeerAddress();
		qint16 getPeerPort();

		void read(const QJsonObject &json);
		void write(QJsonObject &json);

	private:
		QHostAddress address;
		QString name;
		qint16 port;
	};

	/**
	  * @brief type of the socket connection
	  */
	typedef enum {
		TCP,
		UDP,
		UDP_MULTICAST
	}SocketType;

signals:
	void datagramReceiveFromMulticast(QByteArray datagram);

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

	void initMulticast();
};

#endif // MOEDANETWORK_HPP
