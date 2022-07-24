#ifndef _TRIANGLE_HPP_
#define _TRIANGLE_HPP_

#include <bits/stdc++.h>
#include "point.hpp"



class Color {
public:
    double r, g, b;
    Color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Color() {
        this->r = rand() % 255;
        this->g = rand() % 255;
        this->b = rand() % 255;
    }
};


class Triangle {
public:
    vector<Point> points;
    Color color;

    Triangle(Point p1, Point p2, Point p3) {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
    }

    Triangle(Point p1, Point p2, Point p3, Color color) {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        this->color = color;
    }

    Triangle(){
        points.push_back(Point());
        points.push_back(Point());
        points.push_back(Point());
        this->color = Color();
    }

    void print() {
        cout << "Triangle: " << endl;
        for (int i = 0; i < points.size(); i++) {
            points[i].print();
        }
        cout << endl;
    }

    double getMaxX() {
        double max = points[0].x;
        for (int i = 1; i < points.size(); i++) {
            if (points[i].x > max) {
                max = points[i].x;
            }
        }
        return max;
    }

    double getMinX() {
        double min = points[0].x;
        for (int i = 1; i < points.size(); i++) {
            if (points[i].x < min) {
                min = points[i].x;
            }
        }
        return min;
    }

    double getMaxY() {
        double max = points[0].y;
        for (int i = 1; i < points.size(); i++) {
            if (points[i].y > max) {
                max = points[i].y;
            }
        }
        return max;
    }

    double getMinY() {
        double min = points[0].y;
        for (int i = 1; i < points.size(); i++) {
            if (points[i].y < min) {
                min = points[i].y;
            }
        }
        return min;
    }

    Color getColor() {
        return color;
    }

    vector<pair<double,double>> getColumns(double ys, double leftX, double dx){

        double xs, zs;

        vector<pair<double,double>> columns, finalColumns;

        double minX = getMinX();
        double maxX = getMaxX();

        Point a, b, c;

        a = points[0];
        b = points[1];
        c = points[2];

        //edge a-b
        double xa = a.x + ((ys - a.y) / (b.y - a.y)) * (b.x - a.x);
        double za = a.z + ((ys - a.y) / (b.y - a.y)) * (b.z - a.z);
        int s1 = (int) round(abs(leftX - xa) / dx);

        //edge b-c
        double xb = b.x + ((ys - b.y) / (c.y - b.y)) * (c.x - b.x);
        double zb = b.z + ((ys - b.y) / (c.y - b.y)) * (c.z - b.z);
        int s2 = (int) round(abs(leftX - xb) / dx);

        //edge c-a
        double xc = c.x + ((ys - c.y) / (a.y - c.y)) * (a.x - c.x);
        double zc = c.z + ((ys - c.y) / (a.y - c.y)) * (a.z - c.z);
        int s3 = (int) round(abs(leftX - xc) / dx);

        if (!isinf(xa) && !isinf(xb) && !isinf(xc)) {
            if (s1 == s2){
                columns.push_back(make_pair(xa, za));
                columns.push_back(make_pair(xc, zc));
            } else if (s1 == s3){
                columns.push_back(make_pair(xa, za));
                columns.push_back(make_pair(xb, zb));
            } else if (s2 == s3){
                columns.push_back(make_pair(xb, zb));
                columns.push_back(make_pair(xc, zc));
            } else {
                columns.push_back(make_pair(xa, za));
                columns.push_back(make_pair(xb, zb));
                columns.push_back(make_pair(xc, zc));
            }
        } else {
            if (!isinf(xa)) columns.push_back({xa, za});
            if (!isinf(xb)) columns.push_back({xb, zb});
            if (!isinf(xc)) columns.push_back({xc, zc});
        }

        for(int i = 0; i < columns.size(); i++){
            xs = columns[i].first;
            zs = columns[i].second;

            if (xs >= minX && xs <= maxX) {
                finalColumns.push_back(make_pair(xs, zs));
            }
        }

        return finalColumns;
    }

    static vector<Triangle> getTriangles(vector<Point> points){
        vector<Triangle> triangles;

        for (int i = 0; i < points.size(); i += 3) {
            vector<Point> triPoints;
            for (int j = 0; j < 3; j++) {
                triPoints.push_back(points[i+j]);
            }
            Triangle t(triPoints[0], triPoints[1], triPoints[2]);
            triangles.push_back(t);
        }
        return triangles;
    }

    
};


#endif // _TRIANGLE_HPP_