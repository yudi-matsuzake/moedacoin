#include "moedacoin.hpp"
#include "ui_moedacoin.h"

MoedaCoin::MoedaCoin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MoedaCoin),
	dbIsUpdated(false),
	walletSuccefullyOpen(false),
	net(new MoedaNetwork),
	mineIcon(":/icons/minet-512x512.png"),
	mineMovie(":/icons/loading.gif")
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

	/*
	 * connects onRequestMiner to the request of a miner
	 */
	connect(net.get(),
		SIGNAL(requestMiner(MCRequestMiner*)),
		this,
		SLOT(onRequestMiner(MCRequestMiner*)));

	/*
	 * connects onResponseMiner
	 */
	connect(net.get(),
		SIGNAL(responseMiner(MCResponseMiner*)),
		this,
		SLOT(onResponseMiner(MCResponseMiner*)));

	/* connects onRequestUpdate to the request of Update*/
	connect(net.get(),
		SIGNAL(requestUpdate(MCRequestUpdate*)),
		this,
		SLOT(onRequestUpdate(MCRequestUpdate*)));

	/*
	 * connects the mining gif to set the icon of the
	 * mining action
	 * connects the mining gif to restart when finished
	 */
	connect(&mineMovie, SIGNAL(frameChanged(int)), this, SLOT(setMineIcon(int)));
	if (mineMovie.loopCount() != -1)
		connect(&mineMovie, SIGNAL(finished()), &mineMovie, SLOT(start()));
}

MoedaCoin::~MoedaCoin()
{
	delete ui;
}

void MoedaCoin::setButtons()
{
	if(walletSuccefullyOpen){
		ui->actionPublicKey->setEnabled(true);
		ui->actionSendMoedacoin->setEnabled(dbIsUpdated);
		ui->centralWidget->setEnabled(dbIsUpdated);
		ui->actionMining->setEnabled(dbIsUpdated);
		ui->actionSaveWallet->setEnabled(dbIsUpdated);

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
	QList<MCTransaction> transactions =
			moedaDB->getAllTransactions();

	QString myPubKey = QString(wallet->writePubKeyToMemBuf().c_str())
				.trimmed();

	int transactionsSize = transactions.size();
	ui->transactionTableWidget->setRowCount(transactionsSize);
	for (int i = 0; i < transactionsSize; i++){

		MCTransaction transaction = transactions.at(i);
		QString fromKey = transaction.getFromKey();
		QString toKey = transaction.getToKey();
		QString minerKey = transaction.getMinKey();

		QString fromKeyCel = fromKey.split(QChar('\n')).at(2);
		QString subString = fromKeyCel.mid(0, 6);
		fromKeyCel = subString + "...";

		QString toKeyCel = toKey.split(QChar('\n')).at(2);
		subString = toKeyCel.mid(0, 6);
		toKeyCel = subString + "...";

		QString minerKeyCel = minerKey.split(QChar('\n')).at(2);
		subString = minerKeyCel.mid(0, 6);
		minerKeyCel = subString + "...";

		ui->transactionTableWidget->setItem(
			i, 0, new QTableWidgetItem(QString::number(transaction.getId())));
		ui->transactionTableWidget->setItem(
			i, 1, new QTableWidgetItem(fromKeyCel));
		ui->transactionTableWidget->setItem(
			i, 2, new QTableWidgetItem(toKeyCel));
		ui->transactionTableWidget->setItem(
			i, 3, new QTableWidgetItem(QString::number(transaction.getValue())));
		ui->transactionTableWidget->setItem(
			i, 4, new QTableWidgetItem(minerKeyCel));

		QFont font;
		font.setBold(true);
		if(myPubKey == transaction.getFromKey().trimmed())
			ui->transactionTableWidget->item(i, 1)->setFont(font);
		else if(myPubKey == transaction.getToKey().trimmed())
			ui->transactionTableWidget->item(i, 2)->setFont(font);
		else if(myPubKey == transaction.getMinKey().trimmed())
			ui->transactionTableWidget->item(i, 4)->setFont(font);
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

		/*
		 * set old local db for the synched[1] new one.
		 *
		 * [1] http://bit.ly/21nremo
		 */
		moedaDB->setFromBase64(response->getDbData());
		this->atualizeTable();
		dbIsUpdated = true;
	}
	updateMCCLabel();
	setButtons();

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

void MoedaCoin::onResponseMiner(MCResponseMiner* response)
{
	qDebug() << "onResponseMiner";
	if(!response->getAccepted()){
		qDebug() << "onResponseMiner: transaction was not accepted!";
		QMessageBox::critical(this,
				tr("The MoedaCoin Network does not "
				   "accepted your transaction!"),
				response->getReason());
	}else{
		qDebug() << "onResponseMiner: transaction was accepted!";
		MCTransaction transaction = response->getTransaction();
		MCSignature signature = response->getSignature();

		MCWallet minerWallet;
		minerWallet.readPubKeyFromMem(transaction.getMinKey().toUtf8().data());

		if(signature.verifySignature(minerWallet)){
			qDebug() << "onResponseMiner: The miner signed";
			qDebug() << "Requesting a database update";
			MCRequestUpdate* r = new MCRequestUpdate(transaction);
			net->send(r);
		}else{
			qDebug() << "onResponseMiner: The miner did not sign";
		}

	}
}

MCResponseMiner* MoedaCoin::mine(MCRequestMiner* request)
{
	MCResponseMiner* response = new MCResponseMiner(request);
	response->setAccepted(true);

	/*
	 * build the transaction
	 */
	MCTransaction transaction = request->getTransaction();
	QString pubKey(wallet->writePubKeyToMemBuf().c_str());
	transaction.setMinKey(pubKey);

	/*
	 * build the signature
	 */
	QJsonObject jTransaction;
	transaction.write(jTransaction);
	QJsonDocument jdoc(jTransaction);
	QByteArray buffer = jdoc.toJson(QJsonDocument::Compact);
	MCSignature signature = MCCrypto::signature(
					*wallet,
					(unsigned char*)buffer.data(),
					buffer.size());

	response->setTransaction(transaction);
	response->setSignature(signature);

	return response;
}

void MoedaCoin::onRequestMiner(MCRequestMiner* request)
{
	qDebug() << "miner request received";
	bool mining = this->ui->actionMining->isChecked();

	if(mining){
		qDebug() << "im mining";
		MCTransaction transaction = request->getTransaction();
		MCSignature signature = request->getSignature();

		/*
		 * build the signed transaction
		 */
		MCTransaction signedTransaction;
		std::vector<unsigned char> v = signature.getMsg();
		QByteArray binaryTransactionJson((char*)v.data(), v.size());
		QJsonDocument transactionDoc =
				QJsonDocument::fromJson(binaryTransactionJson);
		QJsonObject transactionJson = transactionDoc.object();
		signedTransaction.read(transactionJson);

		/*
		 * build the from/to wallets
		 */
		MCWallet fromWallet;
		MCWallet toWallet;

		fromWallet.readPubKeyFromMem(transaction.getFromKey().toUtf8().data());
		toWallet.readPubKeyFromMem(transaction.getToKey().toUtf8().data());

		std::string myKey = wallet->pubKeyToString();
		std::string fromKey = fromWallet.pubKeyToString();
		std::string toKey = toWallet.pubKeyToString();
		QString fromKeyPEM = transaction.getFromKey();

		/*
		 * begin to test the condition
		 * self conditions
		 */
		if(myKey == fromKey || myKey == toKey){
			qDebug() << "im equal to the fromKey or toKey";
			return;
		}

		/*
		 * valid transaction conditions
		 */
		bool accepted;
		QString reason;
		if(toKey == fromKey){
			QString r = tr("The transaction cannot have the same key "
				    "to send and receive the value!");
			qDebug() << r;

			accepted = false;
			reason = r;
		}else if(transaction.getValue() <= 0.0){
			QString r = tr("The MoedaCoin Network does not accept "
				       "transactions values equal to zero or less.");
			qDebug() << r;

			accepted = false;
			reason = r;
		}else if(moedaDB->getWalletBalance(fromKeyPEM) < transaction.getValue()){
			QString r = tr("A wallet must to have the moedacoins to send them!");
			qDebug() << r;

			accepted = false;
			reason = r;
		}else if(signedTransaction != transaction){
			QString r = tr("Signed transaction is diferent from "
				       "proposed transaction!");
			qDebug() << r;

			accepted = false;
			reason = r;
		}else if(!signature.verifySignature(fromWallet)){
			QString r = tr("Signature is invalid. "
				       "From key does not authorize this transaction!");
			qDebug() << r;

			accepted = false;
			reason = r;
		}else{
			accepted = true;
		}
		qDebug() << "accepted " << accepted;

		/*
		 * build the response and sends it
		 */
		if(!accepted){
			MCResponseMiner* response = new MCResponseMiner(request);
			response->setAccepted(false);
			response->setReason(reason);
			net->send(response);
		}else{
			// mine the transaction
			MCResponseMiner* response = mine(request);
			net->send(response);
		}
	}else{
		qDebug() << "im not mining";
	}

}

void MoedaCoin::onRequestUpdate(MCRequestUpdate* request){

	qDebug() << "Updated needed!!";

	MCTransaction t = request->getTransaction();


	qDebug() << "Calling function to atualize db";

	moedaDB->addNewTransaction(t.getFromKey(), t.getToKey(), t.getMinKey(), t.getValue());

	qDebug() << "Db Updated";

	atualizeTable();
	updateMCCLabel();
}

void MoedaCoin::on_actionSaveWallet_triggered()
{
	QString wallet_filename = QFileDialog::getSaveFileName(
				this,
				tr("Save Moedacoin Wallet"),
				QDir::current().path(), tr("Moedacoin Wallet *.mcwallet"));

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
				QDir::current().path(), tr("Moedacoin Wallet *.mcwallet"));
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
	std::string pubKey(pubk.substr(0, 8) + ".sqlite");
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
		qDebug() << "sendcoinddialog: accepted!";
		QString toPubKey = dialog->getPubKey();
		double value = dialog->getValue();

		qDebug() << "send " << value << " to pubkey: ";
		qDebug() << toPubKey;

		/*
		 * build the transaction
		 */
		qDebug() << "building the transaction";
		MCTransaction transaction;

		std::string pubKeyStdString = wallet->writePubKeyToMemBuf();
		transaction.setFromKey(QString(pubKeyStdString.c_str()));
		transaction.setToKey(toPubKey);
		transaction.setValue(value);

		// TODO: suposted id??
		// transaction.setId();

		/*
		 * build the signature
		 */
		qDebug() << "building the signature";
		QJsonObject jTransaction;
		transaction.write(jTransaction);

		QByteArray buf = QJsonDocument(jTransaction)
				.toJson(QJsonDocument::Compact);

		MCSignature signature = MCCrypto::signature(
					*wallet, (unsigned char*)buf.data(), buf.size());

		/*
		 * build the request
		 */
		qDebug() << "building the request";
		MCRequestMiner* request = new MCRequestMiner(transaction, signature);
		QJsonObject jRequest;
		request->write(jRequest);
		qDebug().noquote() << QJsonDocument(jRequest)
				      .toJson(QJsonDocument::Indented);

		net->send(request);
	}
}

void MoedaCoin::updateMCCLabel()
{
	QString pubkey(wallet->writePubKeyToMemBuf().c_str());
	float mcc = moedaDB->getWalletBalance(pubkey);

	QString labelText = QString("%1 MCC")
				.arg(QString::number(mcc));

	ui->userMCCLabel->setText(labelText);
}


void MoedaCoin::on_actionAbout_triggered()
{}

void MoedaCoin::setMineIcon(int)
{
	ui->actionMining->setIcon(mineMovie.currentPixmap());
}

void MoedaCoin::on_actionMining_toggled(bool tongle)
{
	if(tongle){
		mineMovie.start();
	}else{
		mineMovie.stop();
		ui->actionMining->setIcon(mineIcon);
	}
}
