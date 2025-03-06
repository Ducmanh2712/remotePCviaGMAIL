#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include "../Source/__inIt__.hpp"


class MainWindow : public QMainWindow {
    Q_OBJECT  // Macro cần thiết để Qt xử lý tín hiệu và khe

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setSender(string sender);
    ~MainWindow();

private slots:
    void Help();
    void IPConfig();
    void getFile();
    void deleteFile();
    void handleSend();
    void screenshot();
    void webcam();
    void Shutdown();
    void Restart();
private:
    string sender;
    string currentCommand; // Lưu trữ trạng thái câu lệnh hiện tại
    Config config = loadConfig("../config.json");
    QWidget *centralWidget;       // Widget chính
    QVBoxLayout *buttonLayout;    // Layout chứa các nút
    QPushButton *helpButton, *ipConfigButton, *getFileButton, *deleteFileButton;
    QPushButton *screenCapButton, *webCamButton, *shutdownButton, *restartButton;
    QTextEdit *textDisplay;
    QLineEdit *editbox;
    string editBoxInsert;
};

#endif // MAINWINDOW_HPP
