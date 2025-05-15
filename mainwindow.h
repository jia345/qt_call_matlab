#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QProcess>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void runMatlab();
    void browseScriptFile();
    void readMatlabOutput();
    void processMatlabFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processErrorOccurred(QProcess::ProcessError error);
 
private:
    // UI elements
    QLineEdit *scriptPathEdit;
    QLineEdit *matlabPathEdit;
    QPushButton *browseScriptButton;
    QPushButton *browseMatlabButton;
    QPushButton *runButton;
    QTextEdit *outputText;
    
    // MATLAB Process
    QProcess *matlabProcess;
    
    void browseMatlabExecutable();
};

#endif // MAINWINDOW_H
