#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QPair<QJsonArray, bool> conf_file();
    void show_on_window(QString t);
    QString scanning(QString folderpath);
    QString getpath();
    void reporting(QVector<QString> vectext);
    QString choosingfolder();

private slots:
    void on_finish_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
