#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QString>
#include <QDataStream>
#include <QIODevice>
#include <QTextStream>
#include <QDirIterator>
#include <QPair>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVector>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//--------------------------------------------------
QString MainWindow::getpath()
{
    QString p=QFileDialog::getExistingDirectory(this, tr("Scanning directory"), "/home",
              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    return p;
}
//--------------------------------------------------
void MainWindow::reporting(QVector<QString> vectext)
{
    QFile repfile("C:/users/mbuni/desktop/report.txt");
    if(repfile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream repstream(&repfile);
        foreach(const QString & text, vectext)
        {
            repstream<<text<<"\n";
        }
        repfile.close();
    }
    else
    {
        qDebug()<<"Error: not write the report";
    }
}
//----------------------------------------------------
void MainWindow::show_on_window(QString t)
{
    ui->bytes_area->setText(t);
}
//------------------------------------------------------
QPair<QJsonArray, bool> MainWindow::conf_file()
{
    bool err=false;
    QJsonArray susparr;
    QFile conf("D:/QT-projekty/Wdetector/config.json");
    if(!conf.open(QIODevice::ReadOnly))
    {
       err=true;
    }
    else
    {
        QByteArray bindata=conf.readAll();
        QJsonDocument confdata=QJsonDocument::fromJson(bindata);
        susparr=confdata.array();
    }

    QPair<QJsonArray, bool> configuration;
    configuration.first=susparr;
    configuration.second=err;
    conf.close();

    return configuration;
}
//------------------------------------------------------
QString MainWindow::scanning(QString folderpath)        //---------------------additional arg
{
    QVector<QString> reptext;
    QString repline;
    QJsonArray susparray;
    QPair<QJsonArray, QString> confresult=conf_file();

    if(confresult.second==true)
    {
        return "Error: not open config file";
    }
   else
    {
        susparray=confresult.first;
    }

    QVector<QString> susptext;
    QString oneval;
    QByteArray binval;

    foreach(const QJsonValue & val, susparray)
    {
        oneval=val.toString();
        binval=oneval.toUtf8();
        susptext.push_back(binval);
    }

    int lentext=susptext[0].size();
    foreach(const QString & spt, susptext)
    {
        if(spt.size()>lentext)
        {
            lentext=spt.size();
        }
    }

    //--------------------------------------------------------
    QString path=folderpath;
    QString usepath;
    QDirIterator scanfile(path, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs, QDirIterator::Subdirectories);

    while(scanfile.hasNext())
    {
            scanfile.next();
            usepath=scanfile.filePath();
            QFileInfo checkingfile(usepath);
            qint64 filesize=checkingfile.size();

            bool ifbreak=false;
            QFile filebytes(usepath);

            if(!filebytes.open(QIODevice::ReadOnly))
            {
                repline="Error: not open suspicious file"+usepath;
                reptext.push_back(repline);
            }
            else
            {
                    QDataStream readedbytes(&filebytes);
                    QString hexbuf;
                    QString nextchars="";
                    quint8 onebyte;
                    int intbuf;

                    while(!readedbytes.atEnd())
                    {
                        while((!readedbytes.atEnd())&&(hexbuf.size()<=1000000000))
                        {
                            readedbytes>>onebyte;
                            intbuf=onebyte;
                            hexbuf+=QString::number(intbuf, 16);
                        }

                        hexbuf=nextchars+hexbuf;

                        for(int i=0; i<susptext.size(); i++)
                        {
                            if((susptext[i].size()<=hexbuf.size())&&(hexbuf.contains(susptext[i])==true))
                            {
                                repline="Detected: "+usepath;
                                reptext.push_front(repline);
                                ifbreak=true;
                                break;
                            }
                        }

                        int posit=sizeof(hexbuf)-lentext-1;

                        if((ifbreak==true)||(posit<0))
                        {
                            break;
                        }
                        nextchars=hexbuf.mid(posit, lentext);
                        hexbuf.clear();
                    }
                filebytes.close();
            }
    }

    if((repline.startsWith("Error")==true)||(repline.isEmpty()))
    {
        reptext.push_back("Everything is OK");
    }
    reporting(reptext);

    return "Scanning complete. Let check the report." ;
}


void MainWindow::on_finish_clicked()
{
    qApp->quit();
}

