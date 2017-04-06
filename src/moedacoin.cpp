#include "moedacoin.hpp"
#include "ui_moedacoin.h"

MoedaCoin::MoedaCoin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MoedaCoin),
	dbIsUpdated(false),
	walletSuccefullyOpen(false),
	net(new MoedaNetwork)
{
	ui->setupUi(this);
	setButtons();
	initTable();

	/*
	 * connects onResponseDB to the response of DB
	 */
	connect(net.get(),
		SIGNAL(responseDB(MCRequestDB*, MCResponseDB*)),
		this,
		SLOT(onResponseDB(MCRequestDB*, MCResponseDB*)));

	/*
	 * connects onRequestDB to the request of DB
	 */
	connect(net.get(),
		SIGNAL(requestDB(MCRequestDB*)),
		this,
		SLOT(onRequestDB(MCRequestDB*)));

}

MoedaCoin::~MoedaCoin()
{
	delete ui;
}

void MoedaCoin::setButtons()
{
	if(walletSuccefullyOpen){
		ui->centralWidget->setEnabled(true);
		ui->actionMining->setEnabled(true);
		ui->actionSaveWallet->setEnabled(true);
		ui->actionSendMoedacoin->setEnabled(true);
		ui->actionPublicKey->setEnabled(true);

		qDebug() << "Setting buttons: enable";

	}else{
		ui->centralWidget->setEnabled(false);
		ui->actionMining->setEnabled(false);
		ui->actionSaveWallet->setEnabled(false);
		ui->actionSendMoedacoin->setEnabled(false);
		ui->actionPublicKey->setEnabled(false);

		qDebug() << "Setting buttons: disable";

	}

}

void MoedaCoin::initTable()
{
	QStringList headers;
	ui->transactionTableWidget->setRowCount(0);
	ui->transactionTableWidget->setColumnCount(5);

	ui->transactionTableWidget
		->horizontalHeader()
		->setSectionResizeMode(QHeaderView::Stretch);

	ui->transactionTableWidget
		->verticalHeader()
		->setVisible(false);

	ui->transactionTableWidget
		->setEditTriggers(QAbstractItemView::NoEditTriggers);

	headers <<	tr("ID")
		<<	tr("From")
		<<	tr("To")
		<<	tr("Value")
		<<	tr("Miner");

	ui->transactionTableWidget
		->setHorizontalHeaderLabels(headers);

}

void MoedaCoin::atualizeTable()
{
	QList<Transaction> transactions =
			moedaDB->getAllTransactions();

	int transactionsSize = transactions.size();
	ui->transactionTableWidget->setRowCount(transactionsSize);
	for (int i = 0; i < transactionsSize; i++){
		ui->transactionTableWidget->setItem(
			i, 0, new QTableWidgetItem(QString::number(transactions.at(i).getId())));
		ui->transactionTableWidget->setItem(
			i, 1, new QTableWidgetItem(transactions.at(i).getFromKey()));
		ui->transactionTableWidget->setItem(
			i, 2, new QTableWidgetItem(transactions.at(i).getToKey()));
		ui->transactionTableWidget->setItem(
			i, 3, new QTableWidgetItem(QString::number(transactions.at(i).getValue())));
		ui->transactionTableWidget->setItem(
			i, 4, new QTableWidgetItem(transactions.at(i).getMinKey()));
	}
}

void MoedaCoin::onResponseDB(
	MCRequestDB* request,
	MCResponseDB* response)
{
	if(!response){
		qDebug() << "IEY! I'm updated (probabily)";
		dbIsUpdated = true;
	}else{
		qDebug() << "Updating db...";
		QJsonObject j;
		response->write(j);
		qDebug().noquote()
			<< "response_db: "
			<< QJsonDocument(j).toBinaryData();

		moedaDB->setFromBase64(response->getDbData());
		this->atualizeTable();
	}

}

void MoedaCoin::onRequestDB(MCRequestDB* request)
{
	qDebug() << "MoedaCoin: onRequestDB";
	if(dbIsUpdated){
		qDebug() << "Sending my DB";
		MCResponseDB* response = new MCResponseDB(request);
		response->setDbData(moedaDB->toBase64());
		net->send(response);
	}else{
		qDebug() << "My DB is not updated :(";
	}
}

void MoedaCoin::on_actionSaveWallet_triggered()
{
	QString wallet_filename = QFileDialog::getSaveFileName(
				this,
				tr("Save Moedacoin Wallet"),
				"/home", tr("Moedacoin Wallet *.mcwallet"));

	if(!wallet_filename.endsWith(".mcwallet")){
		wallet_filename += ".mcwallet";
	}

	qDebug() << "onSave: " << wallet_filename;
	wallet->write(wallet_filename.toStdString());
}

void MoedaCoin::on_actionOpenWallet_triggered()
{
	QString wallet_filename = QFileDialog::getOpenFileName(
				this,
				tr("Open Moedacoin Wallet"),
				"/home", tr("Moedacoin Wallet *.mcwallet"));
	qDebug() << "onOpen: " << wallet_filename;

	QFileInfo file(wallet_filename);

	if(file.exists() && !file.isDir()){
		openWallet(wallet_filename);
		walletSuccefullyOpen = true;
		createNewBD();
		this->atualizeTable();
		setButtons();
	}
}

void MoedaCoin::on_actionNewWallet_triggered()
{
	int result = QMessageBox::warning(
					this,
					tr("Create a new wallet"),
					tr("Do you really want to create a new wallet?"),
					QMessageBox::Yes|QMessageBox::No);

	if(result == QMessageBox::Yes){
		if(wallet.get() == NULL ||
				(wallet.get() != NULL &&
				alreadyOpenWalletWarning() == QMessageBox::Yes)){

			generateNewWallet();
			walletSuccefullyOpen = true;
			createNewBD();
			this->atualizeTable();
			setButtons();
		}
	}
}

int MoedaCoin::alreadyOpenWalletWarning()
{
	return QMessageBox::warning(
				this,
				tr("You already have a wallet open!"),
				tr("Do you really want to continue"
				   "(no progress will be saved)?"),
				QMessageBox::Yes|QMessageBox::No);
}

void MoedaCoin::generateNewWallet()
{
	qDebug() << "generate new wallet";
	std::unique_ptr<MCWallet> new_wallet(
				new MCWallet());

	new_wallet->generateKeys();
	wallet = std::move(new_wallet);
}

void MoedaCoin::openWallet(QString &filename)
{
	qDebug() << "open a file " << filename;
	std::unique_ptr<MCWallet> new_wallet(
				new MCWallet(filename.toStdString()));

	wallet = std::move(new_wallet);
}

void MoedaCoin::createNewBD()
{
	std::string pubk = wallet->pubKeyToString();
	std::string pubKey(pubk.substr(0, 4) + ".sqlite");
	qDebug() << pubKey.c_str();

	std::unique_ptr<MCDB> tempDB(new MCDB(
					QString(pubKey.c_str())));

	moedaDB = std::move(tempDB);

	MCRequestDB* r = new MCRequestDB();
	net->send(r);
}

void MoedaCoin::on_actionPublicKey_triggered()
{
	qDebug() << "showing the public key:";
	qDebug() << QString::fromStdString(wallet->pubKeyToString());

	std::string pubkey_str = wallet->writePubKeyToMemBuf();

	QString pubkey = QString::fromStdString(pubkey_str);

	std::unique_ptr<PublicKeyViewDialog> dialog(
				new PublicKeyViewDialog(this, pubkey));

	dialog->exec();
}

void MoedaCoin::on_actionSendMoedacoin_triggered()
{
	std::unique_ptr<SendCoinDialog> dialog(
			new SendCoinDialog());

	if(dialog->exec() == QDialog::Accepted){
		QString pubkey = dialog->getPubKey();
		float value = dialog->getValue();

		qDebug() << "send " << value << " to pubkey: ";
		qDebug() << pubkey;
	}
}

void MoedaCoin::on_actionAbout_triggered()
{
}
