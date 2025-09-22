#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Globalne promenljive
Mat original, hsv, maska, rezultat, siva;
Scalar selektovanaBoja;
bool prozorOtvoren = false; // Flag za otvaranje drugog prozora

// Funkcija za klik
void onMouse(int event, int x, int y, int, void*)
{
    if (event != EVENT_LBUTTONDOWN)
        return;

    // Proveri granice slike
    int regionSize = 5;
    int half = regionSize / 2;

    int hSum = 0, sSum = 0, vSum = 0, count = 0;

    for (int i = -half; i <= half; ++i)
    {
        for (int j = -half; j <= half; ++j)
        {
            int px = x + j;
            int py = y + i;

            if (px >= 0 && px < hsv.cols && py >= 0 && py < hsv.rows)
            {
                Vec3b piksel = hsv.at<Vec3b>(py, px);
                hSum += piksel[0];
                sSum += piksel[1];
                vSum += piksel[2];
                count++;
            }
        }
    }

    int h = hSum / count;
    int s = sSum / count;
    int v = vSum / count;

    selektovanaBoja = Scalar(h, s, v);

    Scalar lower(h - 10, 50, 50);
    Scalar upper(h + 10, 255, 255);

    inRange(hsv, lower, upper, maska);

    cvtColor(original, siva, COLOR_BGR2GRAY);
    cvtColor(siva, rezultat, COLOR_GRAY2BGR);
    original.copyTo(rezultat, maska);

    // Prikaži selektivno bojenje (otvori prozor ako nije već otvoren)
    if (!prozorOtvoren) {
        namedWindow("Selektivno bojenje", WINDOW_NORMAL);
        resizeWindow("Selektivno bojenje", 600, 400);
        prozorOtvoren = true;
    }

    imshow("Selektivno bojenje", rezultat);
}

int main()
{
    // Tačna putanja do slike (dvostruki \\ ili kosu crtu /)
    original = imread("D:/Prebaceno iz C/Desktop/Diplomski/diplomski/x64/Debug/kuce.jpg");

    if (original.empty()) {
        cout << "Greška pri učitavanju slike. Proveri putanju." << endl;
        return -1;
    }

    // Pretvori u HSV
    cvtColor(original, hsv, COLOR_BGR2HSV);

    // Prikaži original
    namedWindow("Originalna slika", WINDOW_NORMAL);
    resizeWindow("Originalna slika", 600, 400);
    setMouseCallback("Originalna slika", onMouse);

    imshow("Originalna slika", original);
    cout << "Klikni na boju koju želiš da zadržiš.\n";

    waitKey(0);
    destroyAllWindows();
    return 0;
}
