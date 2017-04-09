#include "publickeyviewdialog.hpp"

PublicKeyViewDialog::PublicKeyViewDialog(
		QWidget* parent,
		QString& pubkey)
	:QDialog(parent)
{
	ui.setupUi(this);
	this->pubkey = pubkey;
	ui.publicKeyViewTextEdit->setText(this->pubkey);
	ui.publicKeyViewTextEdit->selectAll();
}
