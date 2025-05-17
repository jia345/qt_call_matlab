#include "mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QFormLayout>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt MATLAB Command Line Demo");
    resize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // MATLAB路径设置组
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
    
    // 脚本路径设置组
    QGroupBox *scriptGroup = new QGroupBox("MATLAB Script", this);
    QHBoxLayout *scriptLayout = new QHBoxLayout(scriptGroup);
    
    QLabel *scriptLabel = new QLabel("Script Path:", this);
    scriptPathEdit = new QLineEdit(this);
    
    // 设置默认脚本路径为calculate_with_parameters.m
    QString defaultScriptPath = "D:/WKS/repos/qt_call_matlab/calculate_with_parameters.m";
    if (QFile::exists(defaultScriptPath)) {
        scriptPathEdit->setText(defaultScriptPath);
    }
    
    browseScriptButton = new QPushButton("Browse...", this);
    
    scriptLayout->addWidget(scriptLabel);
    scriptLayout->addWidget(scriptPathEdit);
    scriptLayout->addWidget(browseScriptButton);
    
    // 通用参数输入组
    QGroupBox *generalParamGroup = new QGroupBox("Function Parameters", this);
    QVBoxLayout *generalParamLayout = new QVBoxLayout(generalParamGroup);
    
    QLabel *paramsHelpLabel = new QLabel("Enter parameters separated by spaces (e.g. '10 5 hello \"quoted text\"'):", this);
    parametersEdit = new QLineEdit(this);
    parametersEdit->setPlaceholderText("Enter parameters here...");
    parametersEdit->setText("10 5"); // 默认参数值
    
    generalParamLayout->addWidget(paramsHelpLabel);
    generalParamLayout->addWidget(parametersEdit);
    
    // 运行按钮
    runButton = new QPushButton("Run MATLAB Script", this);
    
    // 输出显示组
    QGroupBox *outputGroup = new QGroupBox("MATLAB Output", this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);
    outputText = new QTextEdit(this);
    outputText->setReadOnly(true);
    outputLayout->addWidget(outputText);
    
    // 添加所有组件到主布局
    mainLayout->addWidget(matlabGroup);
    mainLayout->addWidget(scriptGroup);
    mainLayout->addWidget(generalParamGroup);
    mainLayout->addWidget(runButton);
    mainLayout->addWidget(outputGroup);
    
    // 连接信号和槽
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
        // First try to terminate gracefully
        matlabProcess->terminate();
        
        // Use a timer to kill the process if termination takes too long
        QTimer::singleShot(1000, [this]() {
            if (matlabProcess && matlabProcess->state() == QProcess::Running) {
                matlabProcess->kill();
            }
        });
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
    
    // 获取脚本所在目录路径和文件名（不带扩展名）
    QFileInfo scriptInfo(scriptPath);
    QString scriptDir = QDir::toNativeSeparators(scriptInfo.absolutePath());
    QString scriptName = scriptInfo.baseName();
    
    outputText->clear();
    outputText->append("Starting MATLAB...");
    
    // Prepare arguments and command
    QStringList arguments;
    arguments << "-batch";       // 批处理模式，完成后自动退出
    
    // 获取参数输入框的参数字符串
    QString paramText = parametersEdit->text().trimmed();
    
    // 构建MATLAB命令
    QString command;
    
    // 首先切换到脚本所在目录
    command = QString("cd('%1'); ").arg(scriptDir);
    
    // 判断是函数还是脚本
    QFileInfo fileInfo(scriptPath);
    QString ext = fileInfo.suffix().toLower();
    
    if (ext == "m") {
        // 读取文件以检查是否为函数
        QFile file(scriptPath);
        bool isFunction = false;
        
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString firstLine = in.readLine();
            // 如果第一行以"function"开头，则为函数文件
            isFunction = firstLine.trimmed().startsWith("function", Qt::CaseInsensitive);
            file.close();
        }
        
        if (isFunction) {
            // 如果是函数，直接调用函数并传递参数
            command += scriptName;
            
            // 修改参数处理方式，添加逗号分隔符
            if (!paramText.isEmpty()) {
                // 将空格替换为逗号，用于简单的参数列表
                QString formattedParams = paramText;
                formattedParams.replace(" ", ",");
                
                command += "(" + formattedParams + ")";
            }
            command += ";";
        } else {
            // 如果是脚本，使用run命令
            command += QString("run('%1');").arg(scriptName);
        }
    } else {
        // 非.m文件，使用run命令
        command += QString("run('%1');").arg(scriptPath);
    }
    
    // 将命令添加到参数
    arguments << command;
    
    // 启动MATLAB进程
    matlabProcess->start(matlabPath, arguments);
    
    // 禁用运行按钮，直到进程结束
    runButton->setEnabled(false);
}

void MainWindow::readMatlabOutput()
{
    QByteArray stdOutput = matlabProcess->readAllStandardOutput();
    if (!stdOutput.isEmpty()) {
        QString text = QString::fromLocal8Bit(stdOutput);
        outputText->append(text);
    }
    
    QByteArray stdError = matlabProcess->readAllStandardError();
    if (!stdError.isEmpty()) {
        QString text = QString::fromLocal8Bit(stdError);
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