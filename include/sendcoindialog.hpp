#ifndef SENDCOINDIALOG_HPP
#define SENDCOINDIALOG_HPP

#include <QDialog>
#include <QDoubleValidator>

namespace Ui {
class SendCoinDialog;
}

class SendCoinDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SendCoinDialog(QWidget *parent = 0);
	~SendCoinDialog();

	QString getPubKey();
	float getValue();

private:
	Ui::SendCoinDialog *ui;
};

#endif // SENDCOINDIALOG_HPP
