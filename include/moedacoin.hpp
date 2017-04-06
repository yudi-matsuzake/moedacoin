#ifndef TOPCOIN_HPP
#define TOPCOIN_HPP

#include <QMainWindow>
#include <QMessageBox>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDebug>
#include <QHeaderView>

#include <memory>
#include <vector>

#include "request.hpp"
#include "moedanetwork.hpp"
#include "mcwallet.hpp"
#include "mcsignature.hpp"
#include "publickeyviewdialog.hpp"
#include "sendcoindialog.hpp"
#include "mcdb.hpp"

namespace Ui {
class MoedaCoin;
}

/** Class to describe the main window of MoedaCoin app
 *
 */
class MoedaCoin : public QMainWindow
{
	Q_OBJECT

public:
	explicit MoedaCoin(QWidget *parent = 0);
	~MoedaCoin();

private slots:

	void on_actionSaveWallet_triggered();
	void on_actionOpenWallet_triggered();
	void on_actionNewWallet_triggered();
	void on_actionPublicKey_triggered();
	void on_actionSendMoedacoin_triggered();
	void on_actionAbout_triggered();

	void onResponseDB(MCRequestDB* request, MCResponseDB* response);
	void onRequestDB(MCRequestDB* request);

private:
	Ui::MoedaCoin *ui;

	bool dbIsUpdated;
	bool walletSuccefullyOpen;

	std::unique_ptr<MCWallet> wallet;

	std::unique_ptr<MCDB> moedaDB;
	std::unique_ptr<MoedaNetwork> net;

	/**
	 * @brief sets the enable property of main window based
	 * on `walletSucessfullyOpen`.
	 */
	void setButtons();

	/**
	 * @brief auxialiar function to inicialize the transactions table
	 * on `walletSucessfullyOpen`.
	 */
	void initTable();

	/**
	 * @brief function to atualize table based on a list of  transactions
	 */
	void atualizeTable();

	/**
	 * @brief generate a new pair of keys and put in `wallet`
	 */
	void generateNewWallet();

	/**
	 * @brief show a message box warning about the existence of an
	 * open wallet
	 * @return the messagebox button
	 */
	int alreadyOpenWalletWarning();

	/**
	 * @brief open a wallet in `filename`
	 * @param filename is a path to the wallet file
	 */
	void openWallet(QString& filename);

	/**
	 * @brief createNewBD function
	 */
	void createNewBD();
};

#endif // TOPCOIN_HPP
