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
	  multicastGroupAddress(MULTICAST_IP),
	  signalMapper(new QSignalMapper(this))
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

	QJsonDocument doc = QJsonDocument::fromBinaryData(datagram);

	qDebug().noquote() << "Receive multicast: " << doc.toJson(QJsonDocument::Indented);
	emit datagramReceiveFromMulticast(datagram);
}

void MoedaNetwork::sendMulticast(QByteArray datagram)
{
	multicastSendSocket.writeDatagram(datagram.data(),
					  datagram.size(),
					  multicastGroupAddress,
					  MULTICAST_PORT);
}

QHostAddress MoedaNetwork::getMachineIP()
{
	for (const auto &address : QNetworkInterface::allAddresses()){
		if ( address.protocol() == QAbstractSocket::IPv4Protocol
			&& address != QHostAddress(QHostAddress::LocalHost))
			return address;
	}

	return QHostAddress();
}

void MoedaNetwork::send(MCRequestDB* request)
{
	MCServer* mcserver = new MCServer(request, this);

	assert(mcserver->listen());
	mcserver->timerStart();
	qDebug() << "Listening for ResponseDB in "
			 << mcserver->serverAddress().toString()
			 << " and port " << mcserver->serverPort();

	/*
	 * get the machine ip and set the MCAddress
	 */
	QHostAddress ip = getMachineIP();
	request->setPeer(ip, 0);
	request->setListeningAddress(ip);
	request->setListeningPort(mcserver->serverPort());

	/*
	 * connect to a mcserver
	 */
	connect(mcserver, SIGNAL(response(MCRequestDB*, MCResponseDB*)),
		this, SIGNAL(dbResponse(MCRequestDB*, MCResponseDB*)));

	/*
	 * send the json
	 */
	QJsonObject j;
	request->write(j);
	QJsonDocument jdoc(j);
	sendMulticast(jdoc.toBinaryData());
}
