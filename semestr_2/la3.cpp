#include <iostream>
#include <iomanip>
#include <clocale>
#include <cmath>
using namespace std;

// рекурсивна€ функци€
double calcn(double& x, int& n, int i, double a) {
    if (i == n) {
        return a;
    }
    return a + calcn(x, n, i + 1, a * (-4) * x * x / ((2 * i + 1) * (2 * i + 2)));
}

double calcy(double& x) {
    return 2 * (cos(x) * cos(x) - 1);
}

int main() {
    setlocale(LC_ALL, "ru");
    double x;
    int n;

    cout << "¬ведите x (0.1 <= x <= 1): ";
    cin >> x;

    if (x < 0.1 || x > 1) {
        cout << "x вне диапазона [0.1, 1].\n";
        return 1;
    }

    cout << "¬ведите n (число членов р€да): ";
    cin >> n; // 15

    while (n < 1) {
        cout << "¬ведите n, не меньше 1: ";
        cin >> n;
    }

    cout << fixed << setprecision(2) << "x = " << x << endl;
    cout << setprecision(6) << "s = " << calcn(x, n, 1, -4 * x * x / 2) << endl;
    cout << "y = " << calcy(x) << endl;

    return 0;
}



