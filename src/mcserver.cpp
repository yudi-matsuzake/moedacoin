#include "mcserver.hpp"

const unsigned int MCServer::WAIT_RESPONSE_MS = 10000;

MCServer::MCServer(MCRequestDB* r, QObject* parent) :
	QTcpServer(parent)
{
	this->request = r;
	connect(this,
		SIGNAL(newConnection()),
		this,
		SLOT(onNewConnection()));
}

MCServer::~MCServer()
{}

void MCServer::onReadyRead()
{
	qDebug() << "MCServer: receive db_response!";
	QTcpSocket* socket = dynamic_cast<QTcpSocket*>(sender());
	QByteArray tmpbuf = socket->readAll();

	QJsonObject json =
		QJsonDocument::fromBinaryData(tmpbuf)
				.object();

	qDebug().noquote() << QJsonDocument(json).toJson
			      (QJsonDocument::Indented);

	MCResponseDB* r = new MCResponseDB;

	if(r->read(json)){
		qDebug() << "MCServer: valid json!";
		socket->disconnectFromHost();
		socket->close();

		close();
		emit response(request, r);
	}else{
		qDebug() << "MCServer: valid json!";
		if(socket && socket->isOpen())
			socket->disconnectFromHost();
		delete r;
	}
}

void MCServer::timerStart()
{
	QTimer::singleShot(
			MCServer::WAIT_RESPONSE_MS,
			this,
			SLOT(onTimeout()));
}

void MCServer::onTimeout()
{
	if(this->isListening()){
		close();
		emit response(request, NULL);
	}
}

void MCServer::onNewConnection()
{
	QTcpSocket* socket = nextPendingConnection();
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}
