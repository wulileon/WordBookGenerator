#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class WorkThread: public QThread {
    Q_OBJECT
public:
    WorkThread(QString ws, QString path);

protected:
    void run();

signals:
    void genFinishSignal(bool success);

private:
    QString ws;
    QString path;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void onFileSelected(QString path);


private slots:
    void on_btnSelectFile_clicked();
    void onFileLoad(QString path);

    void on_btnClear_clicked();

    void on_btnStart_clicked();

    void onGenFinish(bool success);

private:
    void getParentPath(QString path);
    Ui::MainWindow *ui;
    WorkThread *workThread;
    QString savePath;
};
#endif // MAINWINDOW_H
