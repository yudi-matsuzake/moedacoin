#include "sendcoindialog.hpp"
#include "ui_sendcoindialog.h"

SendCoinDialog::SendCoinDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SendCoinDialog)
{
	ui->setupUi(this);

	QDoubleValidator* validator =
		new QDoubleValidator(0.0, 100000000.0, 5, this);

	ui->mccLineEdit->setValidator(validator);
}

SendCoinDialog::~SendCoinDialog()
{
	delete ui;
}

QString SendCoinDialog::getPubKey()
{
	return ui->pubkeyTextEdit->toPlainText();
}

float SendCoinDialog::getValue()
{
	return ui->mccLineEdit->text().toFloat();
}
