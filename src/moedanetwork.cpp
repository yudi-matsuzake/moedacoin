#include "moedanetwork.hpp"

const QString MoedaNetwork::MULTICAST_IP = "239.255.43.21";
const short MoedaNetwork::MULTICAST_PORT = 6060;
const short MoedaNetwork::TTL = 5;

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

	QJsonDocument doc = QJsonDocument::fromBinaryData(datagram);

	QJsonObject json = doc.object();
	qDebug().noquote() << "Receive multicast: " << doc.toJson(QJsonDocument::Indented);

	{
		qDebug() << "MCNetwork: testing for MCRequestDB";
		MCRequestDB* r = new MCRequestDB;
		if(r->read(json)){
			qDebug() << "MCNetwork: emit requestDB";
			emit requestDB(r);
			return;
		}else{
			qDebug() << "MCNetwork: not a valid MCRequestDB";
			delete r;
		}
	}
	{
		qDebug() << "MCNetwork: testing for MCRequestMiner";
		MCRequestMiner* r = new MCRequestMiner;
		if(r->read(json)){
			qDebug() << "MCNetwork: emit requestMiner";
			emit requestMiner(r);
			return;
		}else{
			qDebug() << "MCNetwork: not a valid MCRequestMiner";
			delete r;
		}
	}

	{
		MCRequestUpdate* r = new MCRequestUpdate;
		if (r->read(json)){
			qDebug() << "MCNetwork: emit requestUpdate";
			emit requestUpdate(r);
			return;
		}else{
			qDebug() << "MCNetwork: not a valid MCRequestUpdate";
			delete r;
		}
	}
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

void MoedaNetwork::onResponseDbConnect()
{
	qDebug() << "connected!";
	QTcpSocket* socket = dynamic_cast<QTcpSocket*>(sender());
	assert(socket);

	auto iterator = responseDbMap.find(socket);
	MCResponseDB* request = iterator->second;
	responseDbMap.erase(iterator);

	QJsonObject json;
	request->write(json);

	QJsonDocument doc(json);
	QByteArray buffer(doc.toBinaryData());
	socket->write(buffer);
	socket->disconnectFromHost();
}

void MoedaNetwork::onResponseMiner()
{
	QUdpSocket* socket = dynamic_cast<QUdpSocket*>(sender());
	assert(socket);

	QByteArray datagram;

	while (socket->hasPendingDatagrams()){
		datagram.resize(socket->pendingDatagramSize());
		socket->readDatagram(
				datagram.data(),
				datagram.size());
	}

	/*
	 * build the json
	 */
	auto iterator = requestMinerMap.find(socket);
	MCRequestMiner* request = iterator->second;
	QJsonDocument doc = QJsonDocument::fromBinaryData(datagram);
	QJsonObject json = doc.object();
	MCResponseMiner* response = new MCResponseMiner(request);

	qDebug() << "Miner response: " << doc.toJson(QJsonDocument::Indented);

	if(response->read(json)){
		qDebug() << "miner_response is valid!!!";
		requestMinerMap.erase(iterator);
		socket->close();
		emit responseMiner(response);
	}else{
		qDebug() << "miner_response is invalid!!!";
		delete response;
	}

}

void MoedaNetwork::send(MCResponseDB* response)
{
	qDebug() << "Seding db_response";

	QTcpSocket* socket = new QTcpSocket(this);
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
	connect(socket, SIGNAL(connected()), this, SLOT(onResponseDbConnect()));

	/*
	 * maps socket to response
	 */
	std::pair<QTcpSocket*, MCResponseDB*> pair(socket, response);
	responseDbMap.insert(pair);

	/*
	 * retrieve information about request
	 */
	MCAddress addr = response
			->getRequest()
			->getResponseAddress();

	qDebug() << "Connecting to..."
		 << addr.getAddress() << ":"
		 << addr.getPort();
	socket->connectToHost(addr.getAddress(), addr.getPort());
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
		this, SIGNAL(responseDB(MCRequestDB*, MCResponseDB*)));

	/*
	 * send the json
	 */
	QJsonObject j;
	request->write(j);
	QJsonDocument jdoc(j);
	sendMulticast(jdoc.toBinaryData());
}

void MoedaNetwork::send(MCRequestMiner *request)
{
	QUdpSocket* socket = new QUdpSocket(this);
	assert(socket->bind());
	qDebug() << "Waiting for miner in "
		 << socket->localAddress() << ":"
		 << socket->localPort();
	connect(socket, SIGNAL(readyRead()), this, SLOT(onResponseMiner()));

	/*
	 * maps the socket with the request
	 */
	std::pair<QUdpSocket*,MCRequestMiner*> pair(socket, request);
	requestMinerMap.insert(pair);

	/*
	 * put the waiting port and address in the request
	 */
	MCAddress addressResponse(socket->localAddress(), socket->localPort());
	request->setResponseAddress(addressResponse);

	/*
	 * sends the request
	 */
	QJsonObject j;
	request->write(j);
	QJsonDocument jdoc(j);
	sendMulticast(jdoc.toBinaryData());
}

void MoedaNetwork::send(MCResponseMiner* response)
{
	QUdpSocket* socket = new QUdpSocket(this);
	MCAddress addr = response->getRequest()->getResponseAddress();
	int ttl = 5;
	setsockopt(socket->socketDescriptor(), IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
	socket->connectToHost(addr.getAddress(),
			      addr.getPort(),
			      QAbstractSocket::WriteOnly);

	QJsonObject j;
	response->write(j);
	QJsonDocument jdoc(j);
	QByteArray jbinary(jdoc.toBinaryData());
	socket->write(jbinary);
}

void MoedaNetwork::send(MCRequestUpdate* request)
{

	QJsonObject j;

	if(request->write(j)){
		QJsonDocument jdoc(j);
		sendMulticast(jdoc.toBinaryData());
	}else{
		qDebug() << "Failed to create Update Json";
	}
}
