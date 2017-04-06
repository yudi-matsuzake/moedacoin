#include "mcserver.hpp"

const unsigned int MCServer::WAIT_RESPONSE_MS = 10000;

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
	QByteArray tmpbuf = socket->readAll();

	QJsonObject json =
		QJsonDocument::fromJson(QString(tmpbuf).toUtf8())
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
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}
