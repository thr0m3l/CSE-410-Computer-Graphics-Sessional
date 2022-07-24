#include "triangle.hpp"
#include <bits/stdc++.h>

using namespace std;

int main(){
    Triangle t(Point(0,0,0), Point(1000,0,0), Point(0,1000,0));
    t.print();

    vector<pair<double,double>> cols = t.getColumns(2, -10, 5);

    for (int i = 0; i < cols.size(); i++) {
        cout << cols[i].first << " " << cols[i].second << endl;
    }
    return 0;
}