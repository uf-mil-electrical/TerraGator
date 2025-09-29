#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <iostream>
#include <iomanip> // for std::fixed and std::setprecision

std::atomic<bool> running(true);

// Function to detect and print the average color of the most significant object
void print_object_color(const cv::Mat& frame) {
    cv::Mat gray, blurred, edged;
    
    // Convert to grayscale and apply GaussianBlur to reduce noise
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    // Detect edges using Canny edge detection
    cv::Canny(gray, edged, 50, 150);

    // Find contours (objects) and hierarchy for filtering outer contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edged, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Variables to track the largest contour (most significant object)
    double max_area = 0;
    int largest_contour_index = -1;

    // Iterate over contours to find the largest one
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);

        // Ensure the contour is of a reasonable size to avoid noise
        if (area > 500 && area > max_area) {
            max_area = area;
            largest_contour_index = i;
        }
    }

    if (largest_contour_index != -1) {
        // Get the bounding rectangle of the largest contour
        cv::Rect bounding_box = cv::boundingRect(contours[largest_contour_index]);

        // Crop the detected object from the frame
        cv::Mat object = frame(bounding_box);

        // Calculate the average color of the object
        cv::Scalar avg_color = cv::mean(object);

        double blue = avg_color[0];
        double green = avg_color[1];
        double red = avg_color[2];

        std::string dominant_color;

        // Determine the dominant color by comparing RGB values
        if (red > green && red > blue) {
            dominant_color = "Red";
        } else if (green > red && green > blue) {
            dominant_color = "Green";
        } else if (blue > red && blue > green) {
            dominant_color = "Blue";
        } else {
            dominant_color = "Undetermined/Gray";
        }

        // Print the color and RGB values to the terminal
        std::cout << "Dominant Color: " << dominant_color
                  << " (R: " << std::fixed << std::setprecision(2) << red
                  << ", G: " << green << ", B: " << blue << ")"
                  << std::endl;
    } else {
        std::cout << "No significant object detected." << std::endl;
    }
}

// Function to capture frames from the camera and analyze object color
void update_camera_feed() {
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Error: Could not access the camera." << std::endl;
        return;
    }

    while (running) {
        cv::Mat frame;
        camera >> frame;  // Capture frame

        if (frame.empty()) {
            std::cerr << "Error: Failed to capture image." << std::endl;
            break;
        }

        // Detect the largest object and print its color
        print_object_color(frame);

        usleep(100000);  // Sleep for 100 milliseconds (~10 FPS)
    }
    camera.release();
}

int main() {
    // Start the camera feed and object color detection
    update_camera_feed();

    return 0;
}
