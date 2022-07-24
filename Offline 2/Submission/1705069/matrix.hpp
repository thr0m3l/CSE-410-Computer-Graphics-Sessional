#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include<bits/stdc++.h>
#include "point.hpp"

#define PI 3.14159265358979323846
#define WIDTH 10
#define RAD(x) (x*PI/180)


using namespace std;



class Matrix {
    public:
    vector<vector<double>> matrix;
    
    // Constructor
    // Create a matrix with n rows and m columns
    Matrix(int n, int m) {
        matrix.resize(n);
        for (int i = 0; i < n; i++) {
            matrix[i].resize(m);
        }
    }

    // Create a matrix with n rows and m columns filled with zeros
    Matrix(int n, int m, double z) {
        matrix.resize(n);
        for (int i = 0; i < n; i++) {
            matrix[i].resize(m);
            for (int j = 0; j < m; j++) {
                matrix[i][j] = z;
            }
        }
    }


    //Print the matrix
    void print() {
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Return the number of rows
    int getRows() {
        return matrix.size();
    }

    // Return the number of columns
    int getCols() {
        return matrix[0].size();
    }

    // Return the value at row i and column j
    double get(int i, int j) {
        return matrix[i][j];
    }

    // Set the value at row i and column j
    void set(int i, int j, double v) {
        matrix[i][j] = v;
    }

   
    // Multiply the matrix by a scalar
    void mult(double v) {
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                matrix[i][j] *= v;
            }
        }
    }


    // Multiply the matrix by a matrix
    Matrix mult(Matrix m) {
        Matrix result(matrix.size(), m.getCols());
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < m.getCols(); j++) {
                for (int k = 0; k < matrix[i].size(); k++) {
                    result.matrix[i][j] += matrix[i][k] * m.get(k, j);
                }
            }
        }
        return result;
    }

    // Multiply the matrix by a matrix at row i and column j
    void mult(int i, int j, Matrix m) {
        Matrix temp(matrix.size(), m.getCols());
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < m.getCols(); j++) {
                for (int k = 0; k < matrix[i].size(); k++) {
                    temp.set(i, j, temp.get(i, j) + matrix[i][k] * m.get(k, j));
                }
            }
        }
        matrix = temp.matrix;
    }


    static Matrix Identity(int n) {
        Matrix temp(n, n);
        for (int i = 0; i < n; i++) {
            temp.set(i, i, 1);
        }
        return temp;
    }

    static Matrix Translation(double x, double y, double z){
        Matrix temp = Matrix::Identity(4);
        temp.set(0, 3, x);
        temp.set(1, 3, y);
        temp.set(2, 3, z);
        temp.set(3, 3, 1);
        return temp;
    }

    static Matrix Scaling(double x, double y, double z){
        Matrix temp = Matrix::Identity(4);
        temp.set(0, 0, x);
        temp.set(1, 1, y);
        temp.set(2, 2, z);
        return temp;
    }


    //Rodrigues Formula
    static Point RodriguesFormula(Point rotateAxis, Point rotateVector, double angle){
        Point p1 = rotateAxis*cos(angle);
        double temp = (rotateAxis*rotateVector)*(1-cos(angle));
        Point p2 = rotateVector*temp;
        Point p3 = (rotateVector%rotateAxis)*sin(angle);
        return Point(p1+p2+p3);
    }

    static Matrix Rotation(double angle, Point a){
        a.normalize();
        double angleRad = RAD(angle);

        Point c1 = RodriguesFormula(Point(1, 0, 0), a, angleRad);
        Point c2 = RodriguesFormula(Point(0, 1, 0), a, angleRad);
        Point c3 = RodriguesFormula(Point(0, 0, 1), a, angleRad);

        Matrix temp = Matrix::Identity(4);
        temp.set(0, 0, c1.x);
        temp.set(0, 1, c2.x);
        temp.set(0, 2, c3.x);
        temp.set(1, 0, c1.y);
        temp.set(1, 1, c2.y);
        temp.set(1, 2, c3.y);
        temp.set(2, 0, c1.z);
        temp.set(2, 1, c2.z);
        temp.set(2, 2, c3.z);

        return temp;
    }

    

    static Matrix ProjectionMatrix(double near, double far, double r, double t){
        Matrix temp(4, 4);
        temp.set(0, 0, 1.0 / (r * t));
        temp.set(1, 1, 1.0 / t);
        temp.set(2, 2, -(far + near) / (far - near));
        temp.set(3, 2, -1.0);
        temp.set(2, 3, -2.0 * far * near / (far - near));
        return temp;
    }

    static Matrix ViewportMatrix(double x, double y, double width, double height){
        Matrix temp(4, 4);
        temp.set(0, 0, width / 2.0);
        temp.set(1, 1, height / 2.0);
        temp.set(2, 2, 1.0);
        temp.set(3, 3, 1.0);
        temp.set(0, 3, x + width / 2.0);
        temp.set(1, 3, y + height / 2.0);
        return temp;
    }

    static Point transformPoint(Matrix m, Point p){
        Matrix point = Matrix(4, 1);
        point.set(0, 0, p.x);
        point.set(1, 0, p.y);
        point.set(2, 0, p.z);
        point.set(3, 0, 1);

        Matrix result = m.mult(point);
        Point resultPoint(result.get(0, 0), result.get(1, 0), result.get(2, 0));


        return resultPoint;
        
    }

static Point transformPoint2(Matrix m, Point p){
    
    Point result;

    double a, b, sum;

    for(int i = 0; i < 4; i++){
        sum = 0;
        for(int j = 0; j < 4; j++){
            sum += m.get(i, j) * p.get(j);
        }
        result.set(i, sum);
    }

    result.wScale();
    return result;
    }
};

#endif // MATRIX_HPP_