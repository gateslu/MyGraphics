#include <QApplication>
#include "mygraphics.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

//    MyGraphics w;
//    w.show();

    //���������ʾ
    QDesktopWidget *desktop=QApplication::desktop();
    int width=desktop->width();
    int height=desktop->height();

    MyGraphics w;
//    w.setWindowFlags(Qt::windowMinimizeButtonHint);
    w.move((width-400-w.width())/2,(height-200-w.height())/2);   //width-800-w.width():��Ļ��ȼ�ȥ���ڿ����������800,�ߵ�465Ϊ������
    w.resize(1000, 500);
//    w.resize(w.width(),w.height());
    w.show();

    //Ӧ����ʽ
    QApplication::setStyle(QStyleFactory::create("Cleanlooks"));       //Plastique/Cleanlooks/windows/CDE
    
    return a.exec();
}
