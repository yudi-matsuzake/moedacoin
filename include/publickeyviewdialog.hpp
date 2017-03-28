#ifndef PUBLICKEYVIEWDIALOG_HPP
#define PUBLICKEYVIEWDIALOG_HPP
#include "ui_publickeyviewdialog.h"

class PublicKeyViewDialog : public QDialog
{
public:
	PublicKeyViewDialog(
			QWidget* parent,
			QString& pubkey);

private:
	QString pubkey;
	Ui::PublicKeyViewDialog ui;
};

#endif // PUBLICKEYVIEWDIALOG_HPP
