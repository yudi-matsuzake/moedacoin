#include "moedacoin.hpp"
#include "ui_moedacoin.h"

MoedaCoin::MoedaCoin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MoedaCoin)
{
	ui->setupUi(this);
}

MoedaCoin::~MoedaCoin()
{
	delete ui;
}
