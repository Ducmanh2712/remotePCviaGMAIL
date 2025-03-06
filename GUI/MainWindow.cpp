#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include "../commandHelper/commandHelper.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // Đặt kích thước cửa sổ
    resize(1000, 600); 
    setWindowTitle("Remote Control Via Email");

    // Tạo widget chính
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Đặt màu nền chính
    centralWidget->setStyleSheet("background-color: #F8F9FA;");  // Màu nền trắng nhạt

    // Layout chính
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Tạo layout cho cột tùy chọn lệnh (bên trái)
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(15);  // Khoảng cách giữa các nút

    // Định nghĩa style chung cho nút
 QString buttonStyle = 
    "QPushButton {"
    "   color: white; "
    "   background-color:rgba(40, 133, 225, 0.66); "
    "   border: none; "
    "   border-radius: 4px; "  // Góc bo tròn
    "   padding: 10px 20px; "     
    "   font-size: 14px; "
    "   font-weight: bold; "
    "   min-width: 1px; "    // Đặt chiều rộng tối thiểu
    "   min-height: 2px; "    // Đặt chiều cao tối thiểu
    "} "
    "QPushButton:hover {"
    "   background-color:rgb(104, 146, 187); "
    "} "
    "QPushButton:pressed {"
    "   background-color:rgb(92, 136, 180); "
    "}";

    buttonLayout->addSpacing(58);
    
    // Tạo các nút lệnh
    helpButton = new QPushButton("Help", this);
    helpButton->setStyleSheet(buttonStyle);
    ipConfigButton = new QPushButton("IP Config", this);
    ipConfigButton->setStyleSheet(buttonStyle);
    getFileButton = new QPushButton("Get File", this);
    getFileButton->setStyleSheet(buttonStyle);
    deleteFileButton = new QPushButton("Delete File", this);
    deleteFileButton->setStyleSheet(buttonStyle);
    screenCapButton = new QPushButton("Screen Capture", this);
    screenCapButton->setStyleSheet(buttonStyle);
    webCamButton = new QPushButton("Webcam", this);
    webCamButton->setStyleSheet(buttonStyle);
    shutdownButton = new QPushButton("Shutdown", this);
    shutdownButton->setStyleSheet(buttonStyle);
    restartButton = new QPushButton("Restart", this);
    restartButton->setStyleSheet(buttonStyle);

    // Thêm các nút vào layout
    buttonLayout->addWidget(helpButton);
    buttonLayout->addWidget(ipConfigButton);
    buttonLayout->addWidget(getFileButton);
    buttonLayout->addWidget(deleteFileButton);
    buttonLayout->addWidget(screenCapButton);
    buttonLayout->addWidget(webCamButton);
    buttonLayout->addWidget(shutdownButton);
    buttonLayout->addWidget(restartButton);
    
    buttonLayout->addStretch();  // Đẩy các nút lên trên cùng

    // Tạo khu vực hiển thị chính (bên phải)
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    // Tiêu đề
    QLabel *titleLabel = new QLabel("Remote Control Via Email");
    titleLabel->setStyleSheet(
        "font-size: 24px; "
        "font-weight: bold; "
        "color: #333333; "  // Màu chữ xám đậm
        "padding: 10px;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);

    // Khung hiển thị nội dung
    textDisplay = new QTextEdit();
    textDisplay->setReadOnly(true);
    textDisplay->setStyleSheet(
        "font-size: 16px; "
        "color: #333333; "  // Màu chữ xám đậm
        "background-color: #F0F0F0; "  // Màu nền xám nhạt
        "border: 1px solid #CCCCCC; "
        "border-radius: 8px; "
        "padding: 10px;"
    );

    // Khu vực nút gửi và xóa
    QHBoxLayout *controlLayout = new QHBoxLayout();
    QPushButton *clearButton = new QPushButton("Clear");
    QPushButton *sendButton = new QPushButton("Send");
    clearButton->setStyleSheet(buttonStyle);
    sendButton->setStyleSheet(buttonStyle);
    controlLayout->addWidget(clearButton);
    controlLayout->addWidget(sendButton);

    // Thêm các thành phần vào layout bên phải
    rightLayout->addWidget(titleLabel);
    rightLayout->addWidget(textDisplay, 1);
    rightLayout->addLayout(controlLayout);

    // Gắn layout trái và phải vào giao diện chính
    mainLayout->addLayout(buttonLayout, 1);  // Layout nút bên trái
    mainLayout->addLayout(rightLayout, 3);  // Layout hiển thị bên phải

    // Khởi tạo ô nhập đường dẫn file nhưng ẩn đi
    editbox = new QLineEdit(this);
    editbox->setPlaceholderText("Enter file path here...");
    editbox->setStyleSheet(
        "font-size: 14px; "
        "padding: 5px; "
        "border: 1px solid #CCCCCC; "
        "border-radius: 4px;"
        "color : #333333;"
    );
    editbox->hide(); // Ẩn ban đầu
    rightLayout->addWidget(editbox);


    // Kết nối các nút với các slot
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::handleSend);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::Help);
    connect(ipConfigButton, &QPushButton::clicked, this, &MainWindow::IPConfig);
    connect(getFileButton, &QPushButton::clicked, this, &MainWindow::getFile);
    connect(deleteFileButton, &QPushButton::clicked, this, &MainWindow::deleteFile);
    connect(screenCapButton, &QPushButton::clicked, this, &MainWindow::screenshot);
    connect(webCamButton, &QPushButton::clicked, this, &MainWindow::webcam);
    connect(shutdownButton, &QPushButton::clicked, this, &MainWindow::Shutdown);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::Restart);
};

void MainWindow::setSender(string sender){
    this->sender = sender;
}

void MainWindow::handleSend() {
    if (!editbox->isVisible()) {
        return;  // Nếu editbox không hiển thị, nghĩa là đang gửi lệnh khác -> bỏ qua
    }

    editBoxInsert = editbox->text().toStdString();  // Chuyển từ QString -> std::string
    string tmp_command;
    if (this->currentCommand == "getfile"){
        textDisplay->append("get : " + QString::fromStdString(editBoxInsert) + "\n");  // Hiển thị để kiểm tra
        tmp_command = "getfile -p " + editBoxInsert; // Tạo biến tạm để tránh lỗi(cũng không rõ tại sao lỗi nhưng không có thì máy mình bị lỗi)
        getFileCommand(tmp_command, this->config.accessToken, this->sender);
    }
    else if (this->currentCommand == "deletefile"){
        textDisplay->append("delete : " + QString::fromStdString(editBoxInsert) + "\n");  // Hiển thị để kiểm tra
        tmp_command = "deletefile -p " + editBoxInsert; // Tạo biến tạm để tránh lỗi(cũng không rõ tại sao lỗi nhưng không có thì máy mình bị lỗi)
        deleteFileCommand(tmp_command, this->config.accessToken, this->sender);
    }
    else if (this->currentCommand == "webcam"){
        textDisplay->append("webcam record in: " + QString::fromStdString(editBoxInsert) + "s\n");  // Hiển thị để kiểm tra
        tmp_command = "webcam -t " + editBoxInsert;
        webcamRecorder(tmp_command, this->config.accessToken, this->sender);
    }
    editbox->hide();  // Ẩn ô nhập sau khi gửi lệnh
    this->currentCommand = "";
}

void MainWindow::Help() {
    textDisplay->append("Help clicked: Showing help content...\n");
    helpCommand("help", this->config.accessToken, this->sender);
}

void MainWindow::IPConfig() {
    textDisplay->append("IP Config clicked: Running IP configuration function...\n");
    ipconfigCommand("ipconfig", this->config.accessToken, this->sender);
}

void MainWindow::getFile(){
    textDisplay->append("get file clicked: What is the path to the file?");
    editbox->clear();  // Xóa nội dung trước đó (nếu có)
    editbox->show();
    editbox->setFocus();  // Đặt con trỏ nhập vào editbox
    this->currentCommand = "getfile";
}

void MainWindow::deleteFile(){
    textDisplay->append("delete file clicked: What is the path to the file?");
    editbox->clear();  // Xóa nội dung trước đó (nếu có)
    editbox->show();
    editbox->setFocus();  // Đặt con trỏ nhập vào editbox
    this->currentCommand = "deletefile";
}


void MainWindow::screenshot(){
    textDisplay->append("Screenshot clicked: capture the full screen\n");
    screenCaptureCommand("screencap", this->config.accessToken, this->sender);
}

void MainWindow::webcam(){
    textDisplay->append("Webcam clicked: How long do you want to record?");
    editbox->clear(); // Xoa noi dung truoc do neu co
    editbox->show();
    editbox->setFocus(); // Dat con tro vao editbox
    this->currentCommand = "webcam";
}

void MainWindow::Shutdown(){
    textDisplay->append("Shutdown clicked: Goodbye ( ﾟдﾟ)つ Bye");
    shutdownCommand("SHUTDOWN", this->config.accessToken, this->sender);
}

void MainWindow::Restart(){
    textDisplay->append("Restart clicked: See you later ( ﾟдﾟ)つ Bye");
    restartCommand("RESTART", this->config.accessToken, this->sender);
}

MainWindow::~MainWindow() {}

