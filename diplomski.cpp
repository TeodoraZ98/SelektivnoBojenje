#include <opencv2/opencv.hpp>
#include "cvui.h"

#define WINDOW_NAME "Meni"
#define ORIGINAL_WINDOW "Original"
#define RESULT_WINDOW "Rezultat"

using namespace cv;

Mat originalImage, processedImage, resultImage;
bool imageLoaded = false;
bool processingActive = false;
bool showResultWindow = false;

// Razmera za prikaz slike
double scale = 0.5;

// Funkcija za učitavanje slike
void openImage() {
    originalImage = imread("kuce.jpg");
    if (!originalImage.empty()) {
        imageLoaded = true;
    }
    else {
        std::cout << "Greska pri ucitavanju slike!" << std::endl;
    }
}

// Funkcija za konverziju slike u grayscale
void processImage() {
    if (!imageLoaded) return;
    Mat gray;
    cvtColor(originalImage, gray, COLOR_BGR2GRAY);
    cvtColor(gray, processedImage, COLOR_GRAY2BGR);
    resultImage = processedImage.clone();
    showResultWindow = true;
}

// Klik na original -> selektivno bojenje
void onMouseClick(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN && imageLoaded && processingActive) {
        // Preračunaj klik na originalnu veličinu
        int xFull = static_cast<int>(x / scale);
        int yFull = static_cast<int>(y / scale);

        Vec3b selectedColor = originalImage.at<Vec3b>(yFull, xFull);
        int h, s, v;
        Mat hsv;
        cvtColor(originalImage, hsv, COLOR_BGR2HSV);
        Vec3b hsvColor = hsv.at<Vec3b>(yFull, xFull);

        Scalar lower(hsvColor[0] - 10, 50, 50);
        Scalar upper(hsvColor[0] + 10, 255, 255);

        Mat mask;
        inRange(hsv, lower, upper, mask);

        resultImage = processedImage.clone();
        originalImage.copyTo(resultImage, mask);

        // Prikaz smanjene slike
        Mat prikaz;
        resize(resultImage, prikaz, Size(), scale, scale);
        imshow(RESULT_WINDOW, prikaz);
    }
}

int main() {
    cvui::init(WINDOW_NAME);
    namedWindow(WINDOW_NAME);
    resizeWindow(WINDOW_NAME, 300, 250);

    while (true) {
        Mat gui = Mat(250, 300, CV_8UC3, Scalar(50, 50, 50));

        cvui::text(gui, 20, 20, "Izaberi opciju:");

        if (cvui::button(gui, 20, 50, 200, 30, "Ucitaj sliku")) {
            openImage();
            if (imageLoaded) {
                namedWindow(ORIGINAL_WINDOW);
                Mat prikaz;
                resize(originalImage, prikaz, Size(), scale, scale);
                imshow(ORIGINAL_WINDOW, prikaz);
            }
        }

        if (cvui::button(gui, 20, 90, 200, 30, "Procesuiraj")) {
            processImage();
            processingActive = true;
            if (imageLoaded && showResultWindow) {
                namedWindow(RESULT_WINDOW);
                Mat prikaz;
                resize(resultImage, prikaz, Size(), scale, scale);
                imshow(RESULT_WINDOW, prikaz);
                setMouseCallback(ORIGINAL_WINDOW, onMouseClick);
            }
        }

        if (cvui::button(gui, 20, 130, 200, 30, "Sacuvaj rezultat")) {
            if (!resultImage.empty()) {
                imwrite("rezultat.jpg", resultImage);
                std::cout << "Rezultat sacuvan kao rezultat.jpg" << std::endl;
            }
        }

        if (cvui::button(gui, 20, 170, 200, 30, "Izlaz")) {
            break;
        }

        cvui::imshow(WINDOW_NAME, gui);
        int key = waitKey(20);
        if (key == 27) break; // ESC za izlaz
    }

    destroyAllWindows();
    return 0;
}
