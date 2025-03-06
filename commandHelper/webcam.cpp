#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include <atomic>
#include <mutex>
using namespace std;

mutex mtx;

int extractTimeForWebcam(const std::string& command) {
    std::regex pattern(R"(^webcam\s+-t\s+(\d+))");  // Regex để nhận diện lệnh và thời gian
    std::smatch matches;
    
    if (std::regex_match(command, matches, pattern) && matches.size() > 1) {
        return std::stoi(matches[1]);  // Trả về thời gian được trích xuất dưới dạng số nguyên
    }
    return -1;  // Trả về -1 nếu không đúng định dạng
}


void captureVideo(const std::string& outputFile, int durationSeconds) {
    unique_lock<mutex> lock(mtx);

    cv::VideoCapture cap(0);  // Mở webcam
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open webcam." << std::endl;
        return;
    }

    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::VideoWriter video(outputFile, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(frameWidth, frameHeight));

    std::cout << "Recording for " << durationSeconds << " seconds..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        video.write(frame);
        cv::imshow("Recording", frame);
        if (cv::waitKey(1) == 'q') break;

        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() >= durationSeconds) break;
    }

    cap.release();
    video.release();
    cv::destroyAllWindows();
}