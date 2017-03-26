#ifndef TOPCOIN_HPP
#define TOPCOIN_HPP

#include <QMainWindow>

namespace Ui {
class MoedaCoin;
}

class MoedaCoin : public QMainWindow
{
	Q_OBJECT

public:
	explicit MoedaCoin(QWidget *parent = 0);
	~MoedaCoin();

private:
	Ui::MoedaCoin *ui;
};

#endif // TOPCOIN_HPP
