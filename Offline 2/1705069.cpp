#include <bits/stdc++.h>

#include"matrix.hpp"
#include"point.hpp"

using namespace std;

#define PI 3.14159265358979323846
#define WIDTH 10
#define RAD(x) (x*PI/180)

double eyeX, eyeY, eyeZ;
double lookX, lookY, lookZ;
double upX, upY, upZ;
double fovY, aspectRatio, near, far;

int screenWidth, screenHeight;
double xLimit, yLimit;
double zFrontLimit, zRearLimit;

ofstream stageFile;

vector<Point> loadScene(){
    ifstream file("scene.txt");
    vector<Point> points;

    if (!file.is_open()) {
        cout << "Error opening file" << endl;
        return vector<Point>();
    }

    stageFile.open("stage1.txt");
    if (!stageFile.is_open()) {
        cout << "Error opening file" << endl;
        return vector<Point>();
    }

    file >> eyeX >> eyeY >> eyeZ;
    file >> lookX >> lookY >> lookZ;
    file >> upX >> upY >> upZ;
    file >> fovY >> aspectRatio >> near >> far;


    stack<Matrix> transformations;
    transformations.push(Matrix::Identity(4));
   


    stack<unsigned long long> pushStack;
    string command;

    while(getline(file, command)){
        stringstream ss(command);
        ss >> command;

        if (command == "triangle"){


            Point p1, p2, p3;
            file >> p1.x >> p1.y >> p1.z;
            file >> p2.x >> p2.y >> p2.z;
            file >> p3.x >> p3.y >> p3.z;

            Matrix m = transformations.top();
            Point t1 = Matrix::transformPoint(m, p1);
            Point t2 = Matrix::transformPoint(m, p2);
            Point t3 = Matrix::transformPoint(m, p3);


            stageFile << setprecision(7) << fixed << t1.x << " " << t1.y << " " << t1.z << endl;
            stageFile << setprecision(7) << fixed << t2.x << " " << t2.y << " " << t2.z << endl;
            stageFile << setprecision(7) << fixed << t3.x << " " << t3.y << " " << t3.z << endl;

            points.push_back(t1);
            points.push_back(t2);
            points.push_back(t3);

            stageFile << endl;
    

        } else if (command == "translate"){
            double tx, ty, tz;
            file >> tx >> ty >> tz;

            Matrix m = Matrix::Translation(tx, ty, tz);
            transformations.push(transformations.top().mult(m));

        } else if (command == "scale"){
            
            double sx, sy, sz;
            file >> sx >> sy >> sz;
            Matrix m = Matrix::Scaling(sx, sy, sz);
            transformations.push(transformations.top().mult(m));

        } else if (command == "rotate"){
            double angle, ax, ay, az;
            file >> angle >> ax >> ay >> az;
            Matrix m = Matrix::Rotation(angle, Point(ax, ay, az));
            transformations.push(transformations.top().mult(m));
        } else if (command == "push"){
            pushStack.push(transformations.size());
        } else if (command == "pop"){
            
            if (pushStack.size() == 0) {
                cout << "Error: pop without push" << endl;
                return vector<Point>();
            }
            
            unsigned long long popCount = transformations.size() - pushStack.top();
            pushStack.pop();

            while(popCount > 0){
                transformations.pop();
                popCount--;
            }


        } else if (command == "end"){
            break;
        } else {
            continue;
        }

    }

    stageFile.close();

    return points;

}


vector<Point> viewTransformation(vector<Point> points){
    Point look(lookX, lookY, lookZ);
    Point up(upX, upY, upZ);
    Point eye(eyeX, eyeY, eyeZ);

    // cout << "Eye: " << eye.x << " " << eye.y << " " << eye.z << endl;


    Point l = look - eye;
    l.normalize();

    Point r = l%up;
    r.normalize();

    Point u = r%l;
    u.normalize();

    Matrix t = Matrix::Translation(-eye.x, -eye.y, -eye.z);

    Matrix m = Matrix::Identity(4);
    
    m.set(0,0,r.x);
    m.set(0,1,r.y);
    m.set(0,2,r.z);
    m.set(1,0,u.x);
    m.set(1,1,u.y);
    m.set(1,2,u.z);
    m.set(2,0,-l.x);
    m.set(2,1,-l.y);
    m.set(2,2,-l.z);

    Matrix v = m.mult(t);
    


    stageFile.open("stage2.txt");

    if (!stageFile.is_open()) {
        cout << "Error opening file: stage2.txt" << endl;
        return vector<Point>();
    }
    
    vector<Point> transformedPoints;
    for (int i = 0; i < points.size(); i+=3){
        for(int j = 0; j < 3; j++){
            Point p = Matrix::transformPoint(v, points[i+j]);
            stageFile << setprecision(7) << fixed << p.x << " " << p.y << " " << p.z << endl;
            transformedPoints.push_back(p);
        }
        stageFile << endl;
    }

    stageFile.close();
    return transformedPoints;


}

vector<Point> projectionTransformation(vector<Point> points){
    double fovX = fovY * aspectRatio;

    double t = near * tan(RAD(fovY/2));
    double r = near * tan(RAD(fovX/2));

    Matrix m(4,4);
    m.set(0,0,near/r);
    m.set(1,1,near/t);
    m.set(2,2,-(far+near)/(far-near));
    m.set(2,3, -2*far*near/(far-near));
    m.set(3,2,-1);

    stageFile.open("stage3.txt");
    if (!stageFile.is_open()) {
        cout << "Error opening file: stage3.txt" << endl;
        return vector<Point>();
    }

    // for(int i = 0; i < points.size(); i+=3){
    //     for(int j = 0; j < 3; j++){
    //         cout<<points[i+j].x<<" "<<points[i+j].y<<" "<<points[i+j].z<<endl;
    //     }
    //     cout << endl;
    // }
    

    m.print();

    vector<Point> newPoints;
    for (int i = 0; i < points.size(); i+=3){
        for(int j = 0; j < 3; j++){
            Point p = Matrix::transformPoint2(m, points[i+j]);
            stageFile << setprecision(7) << fixed << p.x << " " << p.y << " " << p.z << endl;
            newPoints.push_back(p);
        }
        stageFile << endl;
    }

    stageFile.close();
    return newPoints;
}



int main(int argc, char const *argv[]) {
    
    vector<Point> stage1 = loadScene();
    vector<Point> stage2 = viewTransformation(stage1);
    vector<Point> stage3 = projectionTransformation(stage2);
}