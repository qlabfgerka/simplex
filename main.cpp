#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Simplex {
    vector<vector<double>> A;
    vector<double> b, c, N, B;
    double v = 0;
};

int help() {
    cout << "Usage: ./main <filename>" << endl;

    return -1;
}

void displayMatrix(vector<vector<double>> matrix) {
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[i].size(); j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void displayVector(vector<double> vector) {
    for (int i = 0; i < vector.size(); i++) {
        cout << vector[i] << " ";
    }
    cout << endl << endl;
}

int findMin(vector<double> vector, bool findIndex) {
    int min = INT_MAX, index = -1;
    for (int i = 0; i < vector.size(); i++) {
        if (vector[i] < min) {
            min = vector[i];
            index = i;
        }
    }

    return findIndex ? index : min;
}

int positive(vector<double> c, vector<double> N) {
    for (int i = 0; i < N.size(); i++) {
        if (c[N[i]] > 0) return N[i];
    }

    return -1;
}

bool insideVector(vector<double> vector, int index) {
    for (int i = 0; i < vector.size(); i++) {
        if (vector[i] == index) return true;
    }
    return false;
}

Simplex pivot(Simplex s, int l, int e) {
    Simplex changed = s;
    changed.b[e] = s.b[l] / s.A[l][e];

    for (int j = 0; j < s.N.size(); j++) {
        if (s.N[j] == e) continue;
        changed.A[e][s.N[j]] = s.A[l][s.N[j]] / s.A[l][e];
    }
    changed.A[e][l] = 1 / s.A[l][e];

    for (int i = 0; i < s.B.size(); i++) {
        if (s.B[i] == l) continue;
        changed.b[s.B[i]] = s.b[s.B[i]] - s.A[s.B[i]][e] * changed.b[e];

        for (int j = 0; j < s.N.size(); j++) {
            if (s.N[j] == e) continue;
            changed.A[s.B[i]][s.N[j]] = s.A[s.B[i]][s.N[j]] - s.A[s.B[i]][e] * changed.A[e][s.N[j]];
        }

        changed.A[s.B[i]][l] = -s.A[s.B[i]][e] * changed.A[e][l];
    }

    changed.v = s.v + s.c[e] * changed.b[e];

    for (int j = 0; j < s.N.size(); j++) {
        if (s.N[j] == e) continue;
        changed.c[s.N[j]] = s.c[s.N[j]] - s.c[e] * changed.A[e][s.N[j]];
    }
    changed.c[l] = -s.c[e] * changed.A[e][l];


    for (int i = 0; i < changed.N.size(); i++) {
        if (changed.N[i] == e) changed.N[i] = l;
    }

    for (int i = 0; i < changed.B.size(); i++) {
        if (changed.B[i] == l) changed.B[i] = e;
    }

    return changed;
}

int initializeSimplex(const string &filename, Simplex &s) {
    ifstream file(filename);
    int n, m, size;

    if (!file.is_open()) {
        return help();
    }

    file >> n >> m;
    size = n + m;

    s.A.resize(size);
    s.b.resize(size);
    s.c.resize(size);
    s.N.resize(n);
    s.B.resize(m);

    for (int i = 0; i < size; i++) {
        s.A[i].resize(size);
        for (int j = 0; j < size; j++) {
            file >> s.A[i][j];
        }
    }

    for (int i = 0; i < size; i++) file >> s.b[i];
    for (int i = 0; i < size; i++) file >> s.c[i];
    for (int i = 0; i < n; i++) s.N[i] = i;
    for (int i = 0; i < n; i++) s.B[i] = i + m;
    s.v = 0;

    if (findMin(s.b, false) < 0) return -1;

    return 0;
}

int simplex(const string &filename) {
    Simplex s;
    int e, l;
    double z;
    vector<double> deltas, results;

    if (initializeSimplex(filename, s) != 0) return help();

    /*displayMatrix(s.A);
    displayVector(s.b);
    displayVector(s.c);
    displayVector(s.N);
    displayVector(s.B);*/

    deltas.resize(s.B.size());

    while (positive(s.c, s.N) != -1) {
        e = positive(s.c, s.N);
        for (int j = 0; j < s.B.size(); j++) {
            if (s.A[s.B[j]][e] > 0) deltas[j] = s.b[s.B[j]] / s.A[s.B[j]][e];
            else deltas[j] = INT_MAX;
        }

        l = findMin(deltas, true);

        if (deltas[l] == INT_MAX || l == -1) return -1;
        else s = pivot(s, s.B[l], e);

        /*cout << "------------------" << endl;
        displayMatrix(s.A);
        displayVector(s.b);
        displayVector(s.c);
        displayVector(s.N);
        displayVector(s.B);
        cout << "------------------" << endl;*/
    }

    results.resize(s.N.size() + s.B.size());
    for (int i = 0; i < s.N.size() + s.B.size(); i++) {
        if (insideVector(s.B, i)) results[i] = s.b[i];
        else results[i] = 0;
    }
    z = s.v;

    displayVector(results);

    cout << z << endl;

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) return help();

    return simplex(argv[1]);
}