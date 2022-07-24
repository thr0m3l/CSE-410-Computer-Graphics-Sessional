#ifndef _Z_BUFFER_ALG_HPP
#define _Z_BUFFER_ALG_HPP

#include <bits/stdc++.h>
#include "point.hpp"
#include "triangle.hpp"

extern int screenWidth, screenHeight;
extern double xLimit, yLimit;
extern double zFrontLimit, zRearLimit;

void zBufferAlg(vector<Point> points) {

    vector<Triangle> triangles = Triangle::getTriangles(points);


    double xLeft = xLimit;
    double xRight = -xLimit;
    double dx = (xRight - xLeft) / screenWidth;

    double yBottom = yLimit;
    double yTop = -yLimit;
    double dy = (yTop - yBottom) / screenHeight;

    double topY = yTop - dy / 2;
    double bottomY = yBottom + dy / 2;
    double leftX = xLeft + dx / 2;
    double rightX = xRight - dx / 2;

    vector<vector<double>> zBuffer;
    vector<double> zRowVector;
    bitmap_image image(screenWidth, screenHeight);


    //Initialize zBuffer
    for (int i = 0; i < screenHeight; i++) {
        zRowVector.clear();
        for (int j = 0; j < screenWidth; j++) {
            zRowVector.push_back(zRearLimit);
            image.set_pixel(i, j, 0, 0, 0);
        }
        zBuffer.push_back(zRowVector);
    }

    int top_scanline, bottom_scanline, left_scanline, right_scanline;
    double maxY, minY, maxX, minX;

    //Iterate over triangles
    for (int i = 0; i < triangles.size(); i++) {
        Triangle triangle = triangles[i];


        //Find where to start scanning from (top, bottom, left, right)
        maxY = triangle.getMaxY();
        top_scanline = maxY > topY ? 0 : (int) round(abs(topY - maxY) / dy);

        minY = triangle.getMinY();
        bottom_scanline = minY < bottomY ? screenHeight - 1 : (int) round(abs(topY - minY) / dy);


        //Scan from top to bottom
        for (int row = top_scanline; row <= bottom_scanline; row++) {
            double ys = topY - row * dy;
            vector<pair<double, double>> columns = triangle.getColumns(ys, leftX, dx);

            double za, zb;
            if (columns.empty()) continue;
            if (columns.size() == 1) {
                minX = columns[0].first;
                maxX = columns[0].first;
                za = columns[0].second;
                zb = columns[0].second;
            } else {
                if (columns[0].first <= columns[1].first) {
                    minX = columns[0].first;
                    maxX = columns[1].first;
                    za = columns[0].second;
                    zb = columns[1].second;
                } else {
                    minX = columns[1].first;
                    maxX = columns[0].first;
                    za = columns[1].second;
                    zb = columns[0].second;
                }
            }

            //Find where to start scanning from (left to right)
            left_scanline = minX < leftX ? 0 : (int) round(abs(leftX - minX) / dx);
            right_scanline = maxX > rightX ? screenWidth - 1 : (int) round(abs(leftX - maxX) / dx);

            double z, inc;
            z = za;
            
            inc = right_scanline > left_scanline ? (zb - za) / (right_scanline - left_scanline) : 0;

            if (abs(maxX - minX) >= 1e-6) {

                //Scan from left to right
                for (int col = left_scanline; col <= right_scanline; col++) {
                    if (z >= zFrontLimit && z <= zRearLimit && z < zBuffer[row][col]) {
                        zBuffer[row][col] = z;
                        Color c = triangle.color;
                        image.set_pixel(col, row, c.r, c.g, c.b);
                    }
                    z += inc;
                }
            }
        }
    }

    // save the image and z buffer
    image.save_image("output.bmp");
    ofstream bufferFile;
    bufferFile.open("z_buffer.txt");
    for (int row = 0; row < zBuffer.size(); row++) {
        for (int col = 0; col < zBuffer[row].size(); col++) {
            if (zBuffer[row][col] < zRearLimit) {
                bufferFile << zBuffer[row][col] << "\t";
            }
        }
        bufferFile << endl;
    }

    // clear the buffer and the image
    for (int i = 0; i < zBuffer.size(); i++) {
        zBuffer[i].clear();
    }
    zBuffer.clear();
    image.clear();
}


#endif // _Z_BUFFER_ALG_HPP