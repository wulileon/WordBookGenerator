#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QErrorMessage>
#include <QMessageBox>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    savePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString sp = "/";
#if defined(Q_OS_WIN32)
    sp = "\\";
#elif defined (Q_OS_LINUX)
    sp = "/";
#endif
    savePath = savePath + sp + "金山词霸生词本.txt";
    ui->labelPath->setText(savePath);

    connect(this, SIGNAL(onFileSelected(QString)), this, SLOT(onFileLoad(QString)));
}

MainWindow::~MainWindow()
{
    delete workThread;
    delete ui;
}


void MainWindow::on_btnSelectFile_clicked()
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setViewMode(QFileDialog::List);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setWindowTitle("选择文件");
    dialog.setDefaultSuffix("txt");
    dialog.setNameFilter("txt(*.txt)");
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList list = dialog.selectedFiles();
        QString fileName = list[0];
        qDebug() << fileName;

        ui->leFilePath->setText(fileName);

        emit onFileSelected(fileName);
    }
}

void MainWindow::onFileLoad(QString path)
{
    qDebug() << "onFileLoad: file: " << path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "open file[" << path << "] error...";
    } else {
        QStringList list;
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            list.append(QString(line));
        }

        ui->teNewWord->setText(list.join(""));

        getParentPath(path);

        file.close();
    }

}

void MainWindow::getParentPath(QString path) {
    QString sp = "/";
#if defined(Q_OS_WIN32)
    sp = "\\";
#elif defined (Q_OS_LINUX)
    sp = "/";
#endif
    QStringList p = path.split(sp);
    p.removeLast();
    QString ppath = p.join(sp);

    savePath = ppath + sp + "金山词霸生词本.txt";
    ui->labelPath->setText(savePath);
}

void MainWindow::on_btnClear_clicked()
{
    ui->teNewWord->setText("");
}

void MainWindow::on_btnStart_clicked()
{
    QString ws = ui->teNewWord->toPlainText();
    if (ws.isEmpty()) {
        QMessageBox::information(
                    this,
                    QString("错误"),
                    QString("单词本不能为空"),
                    QMessageBox::Ok);
    } else {

        if (workThread == nullptr) {
            workThread = new WorkThread(ws, savePath);
            connect(workThread, SIGNAL(genFinishSignal(bool)), this, SLOT(onGenFinish(bool)), Qt::QueuedConnection);
        }
        workThread->start();
    }
}

void MainWindow::onGenFinish(bool success) {
    QMessageBox::information(
                this,
                QString("操作完成"),
                QString("已生成生词本"),
                QMessageBox::Ok);

    workThread->wait();
}

WorkThread::WorkThread(QString ws, QString path) {
    this->ws = ws;
    this->path = path;
}

void WorkThread::run() {
    QStringList list = ws.split("\n");

    QStringList tList;
    QList<QString>::Iterator it = list.begin(),itend=list.end();
    int i = 0;
    while (it != itend) {
        QString w = list.at(i).trimmed();
        i++;
        it++;
        if (!w.isEmpty()) {
            tList.append("+"+w+"\n$1");
        }
    }

    QString ret = tList.join("\n");
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(ret.toUtf8());
        file.flush();
        file.close();
    }

    emit genFinishSignal(true);

}
