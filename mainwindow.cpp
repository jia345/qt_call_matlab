#include "mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt MATLAB Command Line Demo");
    resize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QGroupBox *matlabGroup = new QGroupBox("MATLAB Executable", this);
    QHBoxLayout *matlabLayout = new QHBoxLayout(matlabGroup);
    
    QLabel *matlabLabel = new QLabel("MATLAB Path:", this);
    matlabPathEdit = new QLineEdit(this);
    
    QSettings settings("QtCallMatlab", "MatlabPath");
    QString matlabPath = settings.value("MatlabPath").toString();
    
    if (matlabPath.isEmpty()) {
        QStringList possiblePaths = {
            "C:/Program Files/MATLAB/R2022b/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2022a/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2021b/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2021a/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2020b/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2020a/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2019b/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2019a/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2018b/bin/matlab.exe",
            "C:/Program Files/MATLAB/R2018a/bin/matlab.exe"
        };
        
        for (const QString &path : possiblePaths) {
            if (QFile::exists(path)) {
                matlabPath = path;
                settings.setValue("MatlabPath", matlabPath);
                break;
            }
        }
    }
    
    matlabPathEdit->setText(matlabPath);
    browseMatlabButton = new QPushButton("Browse...", this);
    
    matlabLayout->addWidget(matlabLabel);
    matlabLayout->addWidget(matlabPathEdit);
    matlabLayout->addWidget(browseMatlabButton);
    
    QGroupBox *scriptGroup = new QGroupBox("MATLAB Script", this);
    QHBoxLayout *scriptLayout = new QHBoxLayout(scriptGroup);
    
    QLabel *scriptLabel = new QLabel("Script Path:", this);
    scriptPathEdit = new QLineEdit(this);
    
    // Set default script path
    QString defaultScriptPath = "D:/WKS/repos/qt_call_matlab/example.m";
    if (QFile::exists(defaultScriptPath)) {
        scriptPathEdit->setText(defaultScriptPath);
    }
    
    browseScriptButton = new QPushButton("Browse...", this);
    
    scriptLayout->addWidget(scriptLabel);
    scriptLayout->addWidget(scriptPathEdit);
    scriptLayout->addWidget(browseScriptButton);
    
    runButton = new QPushButton("Run MATLAB Script", this);
    
    QGroupBox *outputGroup = new QGroupBox("MATLAB Output", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    outputText = new QTextEdit(this);
    outputText->setReadOnly(true);
    outputLayout->addWidget(outputText);
    
    mainLayout->addWidget(matlabGroup);
    mainLayout->addWidget(scriptGroup);
    mainLayout->addWidget(runButton);
    mainLayout->addWidget(outputGroup);
    
    connect(browseScriptButton, &QPushButton::clicked, this, &MainWindow::browseScriptFile);
    connect(browseMatlabButton, &QPushButton::clicked, this, &MainWindow::browseMatlabExecutable);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runMatlab);
    
    matlabProcess = new QProcess(this);
    connect(matlabProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readMatlabOutput);
    connect(matlabProcess, &QProcess::readyReadStandardError, this, &MainWindow::readMatlabOutput);
    connect(matlabProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &MainWindow::processMatlabFinished);
    connect(matlabProcess, &QProcess::errorOccurred, this, &MainWindow::processErrorOccurred);
}

MainWindow::~MainWindow()
{
    if (matlabProcess && matlabProcess->state() == QProcess::Running) {
        matlabProcess->terminate();
        matlabProcess->waitForFinished(3000);
    }
}

void MainWindow::browseScriptFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, 
        "Select MATLAB Script", 
        QDir::homePath(), 
        "MATLAB Scripts (*.m);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        scriptPathEdit->setText(filePath);
    }
}

void MainWindow::browseMatlabExecutable()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, 
        "Select MATLAB Executable", 
        "C:/Program Files/MATLAB", 
        "MATLAB Executable (matlab.exe);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        matlabPathEdit->setText(filePath);
        QSettings settings("QtCallMatlab", "MatlabPath");
        settings.setValue("MatlabPath", filePath);
    }
}

void MainWindow::runMatlab()
{
    QString matlabPath = matlabPathEdit->text();
    if (matlabPath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select MATLAB executable path first");
        return;
    }
    
    QString scriptPath = scriptPathEdit->text();
    if (scriptPath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select MATLAB script file first");
        return;
    }
    
    matlabPath = QDir::toNativeSeparators(matlabPath);
    scriptPath = QDir::toNativeSeparators(scriptPath);
    
    outputText->clear();
    outputText->append("Starting MATLAB...");
    
    if (matlabProcess->state() == QProcess::Running) {
        matlabProcess->terminate();
        matlabProcess->waitForFinished(3000);
    }
    
    QStringList arguments;
    arguments << "-nosplash";
    arguments << "-nodesktop";
    arguments << "-batch";
    
    QString command = QString("run('%1'); quit;").arg(scriptPath);
    arguments << command;
    
    matlabProcess->start(matlabPath, arguments);
    
    runButton->setEnabled(false);
}

void MainWindow::readMatlabOutput()
{
    QByteArray stdOutput = matlabProcess->readAllStandardOutput();
    if (!stdOutput.isEmpty()) {
        QString text = QString::fromUtf8(stdOutput);
        outputText->append(text);
    }
    
    QByteArray stdError = matlabProcess->readAllStandardError();
    if (!stdError.isEmpty()) {
        QString text = QString::fromUtf8(stdError);
        outputText->append(text);
    }
}

void MainWindow::processMatlabFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    runButton->setEnabled(true);
    
    if (exitStatus == QProcess::NormalExit) {
        outputText->append(QString("MATLAB process completed, exit code: %1").arg(exitCode));
    } else {
        outputText->append("MATLAB process terminated abnormally");
    }
}

void MainWindow::processErrorOccurred(QProcess::ProcessError error)
{
    runButton->setEnabled(true);
    
    switch (error) {
    case QProcess::FailedToStart:
        outputText->append("Error: MATLAB process failed to start. Please check the MATLAB executable path");
        QMessageBox::critical(this, "Error", "MATLAB process failed to start. Please check the MATLAB executable path");
        break;
    case QProcess::Crashed:
        outputText->append("Error: MATLAB process crashed");
        break;
    case QProcess::Timedout:
        outputText->append("Error: MATLAB process operation timed out");
        break;
    case QProcess::WriteError:
        outputText->append("Error: Could not write to MATLAB process");
        break;
    case QProcess::ReadError:
        outputText->append("Error: Could not read from MATLAB process");
        break;
    default:
        outputText->append("Error: Unknown error with MATLAB process");
        break;
    }
} 