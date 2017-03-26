#include "moedacoin.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MoedaCoin w;
	w.show();

	return a.exec();
}
