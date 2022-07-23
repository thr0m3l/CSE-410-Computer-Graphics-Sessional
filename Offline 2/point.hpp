#ifndef _POINT_HPP_
#define _POINT_HPP_

#include <bits/stdc++.h>

using namespace std;

class Point {
public:
    double x, y, z, w;
    
    Point(double x, double y, double z, double w = 1) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    Point() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->w = 1;
    }

    void wScale() {
        
        this->x /= w;
        this->y /= w;
        this->z /= w;
        this->w = 1;

    }
    
    void normalize(){
        double len = sqrt(x*x + y*y + z*z);
        x /= len;
        y /= len;
        z /= len;
    }

    Point operator+(Point p) {
        return Point(x + p.x, y + p.y, z + p.z);
    }

    Point operator-(Point p) {
        return Point(x - p.x, y - p.y, z - p.z);
    }

    Point operator*(double d) {
        return Point(x * d, y * d, z * d);
    }

    Point operator/(double d) {
        return Point(x / d, y / d, z / d);
    }

    double operator*(Point p) {
        return x * p.x + y * p.y + z * p.z;
    }

    double operator^(Point p) {
        return x * p.y - y * p.x;
    }

    double operator!() {
        return sqrt(x * x + y * y + z * z);
    }

    Point operator~() {
        return Point(-y, x, 0);
    }

    Point operator%(Point p) {
        return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }


    double angle(Point p) {
        return acos(dot(p) / (!*this * !p));
    }


    double dot(const Point &p) const {
        return x * p.x + y * p.y + z * p.z;
    }

    Point cross(const Point &p) const {
        return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }

    double get(int index){
        if(index == 0) return x;
        else if(index == 1) return y;
        else return z;
    }

    void set(int index, double value){
        if(index == 0) x = value;
        else if(index == 1) y = value;
        else z = value;
    }

    Point sub(Point p){
        return Point(x - p.x, y - p.y, z - p.z);
    }

    void print() {
        cout<<"("<<x<<","<<y<<","<<z<<")"<<endl;
    }


};

#endif // _POINT_HPP_