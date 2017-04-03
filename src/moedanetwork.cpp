#include "moedanetwork.hpp"

const QString MoedaNetwork::MULTICAST_IP = "239.255.43.21";
const short MoedaNetwork::MULTICAST_PORT = 45454;
const short MoedaNetwork::TTL = 1;

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

	qDebug() << "emit";
	emit datagramReceiveFromMulticast(datagram);
}

void MoedaNetwork::sendMulticast(QByteArray datagram)
{
	multicastSendSocket.writeDatagram(datagram.data(),
									  datagram.size(),
									  multicastGroupAddress,
									  MULTICAST_PORT);
}

MoedaNetwork::Peer::Peer()
{}

MoedaNetwork::Peer::Peer(
		QHostAddress address,
		QString name,
		qint16 port)
{
	this->address	= address;
	this->port		= port;
	this->name		= name;
}

MoedaNetwork::Peer::~Peer()
{}

QHostAddress MoedaNetwork::Peer::getPeerAddress()
{
	return this->address;
}

qint16 MoedaNetwork::Peer::getPeerPort()
{
	return this->port;
}

void MoedaNetwork::Peer::read(const QJsonObject &json)
{
	this->address	= QHostAddress(json["peerAddress"].toString());
	this->port		= json["peerPort"].toInt();
	this->name		= json["peerAddress"].toString();
}

void MoedaNetwork::Peer::write(QJsonObject &json)
{
	json["peerAddress"]	= this->address.toString();
	json["peerPort"]	= this->port;
	json["peerName"]	= this->name;
}
