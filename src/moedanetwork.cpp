#include "moedanetwork.hpp"

const QString MoedaNetwork::MULTICAST_IP = "239.255.43.21";
const short MoedaNetwork::MULTICAST_PORT = 6060;
const short MoedaNetwork::TTL = 1;

/*
 * moedanetwork
 */
MoedaNetwork::MoedaNetwork()
	: multicastSocket(this),
	  multicastSendSocket(this),
	  multicastGroupAddress(MULTICAST_IP)
{
	initMulticast();
}

void MoedaNetwork::initMulticast()
{

	multicastSocket.bind(QHostAddress::AnyIPv4,
					MoedaNetwork::MULTICAST_PORT,
					QUdpSocket::ShareAddress);

	multicastSocket.joinMulticastGroup(multicastGroupAddress);

	connect(&multicastSocket,
			SIGNAL(readyRead()),
			this,
			SLOT(onReceiveDatagrams()));

	multicastSendSocket.setSocketOption(
				QAbstractSocket::MulticastTtlOption, TTL);
}

void MoedaNetwork::onReceiveDatagrams()
{
	QByteArray datagram;

	while (multicastSocket.hasPendingDatagrams()) {
		datagram.resize(multicastSocket.pendingDatagramSize());
		multicastSocket.readDatagram(datagram.data(), datagram.size());
	}

	qDebug() << "Receive multicast: " << datagram;
	emit datagramReceiveFromMulticast(datagram);
}

void MoedaNetwork::sendMulticast(QByteArray datagram)
{
	multicastSendSocket.writeDatagram(datagram.data(),
									  datagram.size(),
									  multicastGroupAddress,
									  MULTICAST_PORT);
}

/*
 * Response / Request DB
 *
 */
void MoedaNetwork::onResponseDB()
{
	QTcpServer* tcpServer = dynamic_cast<QTcpServer*>(sender());
	assert(tcpServer);

	QTcpSocket* s = tcpServer->nextPendingConnection();
	QDataStream datastream;

	datastream.setDevice(s);
	datastream.setVersion(QDataStream::Qt_5_8);

	datastream.startTransaction();

	QString response;
	datastream >> response;

	qDebug() << "Receive response " << response;

	tcpServer->close();
	delete tcpServer;
}

//void MoedaNetwork::send(MCRequestDB& request)
//{
//	QTcpServer* tcpServer = new QTcpServer(this);

//	assert(tcpServer->listen());
//	qDebug() << "Listening for ResponseDB in "
//			 << tcpServer->serverAddress().toString()
//			 << " and port " << tcpServer->serverPort();

//	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onResponseDB()));
//	QHostAddress h = tcpServer->serverAddress();
//	request.setIp(h);
//	request.setPort(tcpServer->serverPort());

//	QJsonObject j;
//	request.write(j);
//	QJsonDocument jdoc(j);
//	sendMulticast(jdoc.toBinaryData());
//}
