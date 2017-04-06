#include "mcserver.hpp"

const unsigned int MCServer::WAIT_RESPONSE_MS = 20000;

MCServer::MCServer(MCRequestDB* r, QObject* parent) :
	QTcpServer(parent),
	socket(NULL)
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
	QByteArray tmpbuf = socket->readAll();

	QJsonObject json =
		QJsonDocument::fromBinaryData(tmpbuf)
				.object();

	qDebug().noquote() << QJsonDocument(json).toJson
			      (QJsonDocument::Indented);

	MCResponseDB* r = new MCResponseDB;

	if(r->read(json)){
		socket->disconnectFromHost();
		socket->close();

		close();
		emit response(request, r);
	}else{
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
		if(this->socket){
			socket->disconnectFromHost();
			socket->close();
		}

		this->close();

		close();
		emit response(request, NULL);
	}
}

void MCServer::onNewConnection()
{
	socket = nextPendingConnection();
	connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}
