#include "moedacoin.hpp"
#include "ui_moedacoin.h"

MoedaCoin::MoedaCoin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MoedaCoin),
	walletSuccefullyOpen(false)
{
	ui->setupUi(this);
	setButtons();
	initTable();
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

void MoedaCoin::atualizeTable(QList<Transaction> transactions){

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
		std::unique_ptr<MCDB> tempDB(
					new MCDB("padraoDB.sqlite"));
		moedaDB = std::move(tempDB);
		this->atualizeTable(moedaDB->getAllTransactions());
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
			std::unique_ptr<MCDB> tempDB(
					new MCDB("padraoDB.sqlite"));
			moedaDB = std::move(tempDB);
			this->atualizeTable(moedaDB->getAllTransactions());
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

void MoedaCoin::on_actionPublicKey_triggered()
{
	qDebug() << "showing the public key:";
	std::string pubkey_str = wallet->writePubKeyToMemBuf();

	QString pubkey = QString::fromStdString(pubkey_str);
	qDebug() << pubkey;

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
