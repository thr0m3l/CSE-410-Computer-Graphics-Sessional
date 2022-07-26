#include <iostream>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <GL/glut.h>

using namespace std;

#define PI (2*acos(0.0))
#define EPSILON 0.0000001


class Object;
class Light;

extern vector<Object*> objects;
extern vector<Light> lights;
extern int recursion_level;

class Color {
public:
    double r, g, b;
    Color() {r = g = b = 0;}
    Color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    Color& operator=(const Color& c) = default;
    Color operator+(const Color& c) const {
        return {r + c.r, g + c.g, b + c.b};
    }
    Color operator*(double coefficient) const {
        return {r * coefficient, g * coefficient, b * coefficient};
    }
    Color operator*(const Color& c) const {
        return {r * c.r, g * c.g, b * c.b};
    }
    void clip() {
        r = min(r, 1.0);
        g = min(g, 1.0);
        b = min(b, 1.0);

        r = max(r, 0.0);
        g = max(g, 0.0);
        b = max(b, 0.0);
    }
};


class Vector3D {
public:
    double x, y, z;

    Vector3D() {
        x = y = z = 0;
    }
    Vector3D(double x, double y, double z)  {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vector3D(const Vector3D &v) = default;

    Vector3D operator+(const Vector3D &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }
    Vector3D operator-(const Vector3D &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }
    Vector3D operator*(double s) const {
        return {s * x, s * y, s * z};
    }


    Vector3D& operator=(const Vector3D &v) = default;


    Vector3D rotate(const Vector3D& r, double angle) {
        Vector3D res = ((*this) * cos(angle)) + (r.cross(*this) * sin(angle));
        res.normalize();
        return res;
    }

    double length() {
        return sqrt(x * x + y * y + z * z);
    }

    double dot(const Vector3D &b) const{
        return x * b.x + y * b.y + z * b.z;
    }
    Vector3D cross(const Vector3D &b) const {
        return {y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x};
    }

    Vector3D& operator+=(const Vector3D &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    void normalize() {
        double l = length();
        x /= l;
        y /= l;
        z /= l;
    }


};





class Ray {
public:
    Vector3D start{};
    Vector3D dir{};

    Ray(const Vector3D &start, const Vector3D &dir) {
        this->start = start;
        this->dir = dir;
        this->dir.normalize();
    }
};


class Light {
public:
    Vector3D light_pos{};
    Color color{};

    Light(const Vector3D &pos, const Color &c) {
        light_pos = pos;
        color = c;
    }

    void draw();
};

void Light::draw() {
    glBegin(GL_POINTS);{
        glColor3f(color.r, color.g, color.b);
        glVertex3f(light_pos.x, light_pos.y, light_pos.z);
    }glEnd();
}



class Object {
protected:
    Vector3D reference_point;
    double height, width, length;
    Color color;
    double coefficients[4]; // reflection coefficients
    int shine; // exponent term of specular component

public:
    Object()= default;

    virtual void draw(){}

    virtual double intersect(const Ray& r, Color& clr, int level){
        return -1.0;
    }

    virtual Vector3D getNormal(const Vector3D& intersectionPoint) {
        return Vector3D();
    }

    virtual Color getColor(const Vector3D& intersectionPoint) const {
        return color;
    }

    double intersectWithIllumination(const Ray& r, Color& clr, int level) {
        double tMin = this->intersect(r, clr, level);

    if (level == 0) return tMin;

    // Find the intersection point of the ray with the object
    Vector3D ro = r.start; // origin
    Vector3D rd = r.dir; // direction
    Vector3D intersectionPoint = ro + rd * tMin;


    clr = this->getColor(intersectionPoint) * coefficients[0]; // ambient
    clr.clip();
    Vector3D normal = this->getNormal(intersectionPoint);
    normal.normalize();

    for (Light l : lights) {
        // cast ray from light source
        Vector3D light_direction = l.light_pos - intersectionPoint;
        light_direction.normalize();
        
        
        Vector3D light_position = intersectionPoint + light_direction * 0.0000000001;
        Ray lightRay(light_position, light_direction);

        // check if any other object is present between this object & light source
        bool inShadow = false;
        Color temp;
        double t, tMinActual = INFINITY;


        for (Object* o : objects) {
            t = o->intersectWithIllumination(lightRay, temp, 0);
            if (t > 0 && t < tMinActual) {
                tMinActual = t;
            }
        }
        if (tMin > tMinActual) {
            //Obscured by another object, so don't illuminate
            continue;
        } else {

            double lambert = max(normal.dot(light_direction), 0.0);
            // R = 2(L.N)N – L
            Vector3D R = normal * 2.0 * normal.dot(light_direction) - light_direction;
            R.normalize();

            double phong = max(pow((rd.dot(R)), shine), 0.0);
            clr = clr + this->getColor(intersectionPoint) * l.color * lambert * coefficients[1];
            clr.clip();
            clr = clr + l.color * phong * coefficients[2];
            clr.clip();

        }


    }

    // recursive reflection
    if (level >= recursion_level) return tMin;
    // construct reflected ray from intersection point
    Vector3D rayReflectedDir = rd - normal * 2.0 * normal.dot(rd);
    rayReflectedDir.normalize();
    Vector3D rayReflectedStart = intersectionPoint + rayReflectedDir * 0.0000000001;
    Ray rayReflected(rayReflectedStart, rayReflectedDir);

    // find nearest intersecting object
    Color colorReflected;
    int nearest = -1;
    double tReflected, tReflectedMin = INFINITY;
    for (int k = 0; k < objects.size(); k++) {
        tReflected = objects[k]->intersectWithIllumination(rayReflected, colorReflected, 0);
        if (tReflected > 0 && tReflected < tReflectedMin) {
            tReflectedMin = tReflected;
            nearest = k;
        }
    }

    if (nearest != -1) {
        tReflectedMin = objects[nearest]->intersectWithIllumination(rayReflected, colorReflected, level + 1);
        clr = clr + colorReflected * coefficients[3];
        clr.clip();
    }
    return tMin;
    }

    void setColor(double c1, double c2, double c3);
    void setShine(int s);
    void setCoefficients(double c1, double c2, double c3, double c4);

    Color calculatePhong(){
        return Color();
    }


};


void Object::setColor(double c1, double c2, double c3) {
    color.r = c1;
    color.g = c2;
    color.b = c3;
}

void Object::setShine(int s) {
    shine = s;
}

void Object::setCoefficients(double c1, double c2, double c3, double c4) {
    coefficients[0] = c1;
    coefficients[1] = c2;
    coefficients[2] = c3;
    coefficients[3] = c4;
}


class Sphere : public Object {
public:
    Sphere(Vector3D center, double radius) {
        reference_point = center;
        length = radius;
    }

    void draw() override;
    double intersect(const Ray& r, Color& clr, int level) override;
    Vector3D getNormal(const Vector3D &intersectionPoint) override;
};

void Sphere::draw() {
    glTranslatef(reference_point.x, reference_point.y, reference_point.z);
    int stacks = 20, slices = 24;
    Vector3D points[100][100];
    int i,j;
    double h,r;
    double radius = length;
    //generate points
    for(i=0;i<=stacks;i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(PI/2));
        r=radius*cos(((double)i/(double)stacks)*(PI/2));
        for(j=0;j<=slices;j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*PI);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*PI);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0;i<stacks;i++)
    {
        glColor3f(color.r,color.g,color.b);
        for(j=0;j<slices;j++)
        {
            glBegin(GL_QUADS);{
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            }glEnd();
        }
    }
    glTranslatef(-reference_point.x, -reference_point.y, -reference_point.z);
}

double Sphere::intersect(const Ray &r, Color &clr, int level) {
    Vector3D ro = r.start - reference_point; // origin
    Vector3D rd = r.dir; // direction
    double radius = length;
    double a = 1;
    double b = rd.dot(ro) * 2;
    double c = ro.dot(ro) - radius * radius;
    double temp = b * b - 4 * a * c;
    if (temp < 0) return -1;
    double d = sqrt(temp);
    double t1 = (- b - d) / (2 * a);
    double t2 = (- b + d) / (2 * a);

    if (t1 < 0 && t2 < 0) return -1;
    else if (t1 > 0) return t1;
    else if (t2 > 0) return t2;
    return -1;
}

Vector3D Sphere::getNormal(const Vector3D &intersectionPoint) {
    Vector3D n = intersectionPoint - reference_point;
    n.normalize();
    return n;
}

class Triangle : public Object {
    Vector3D points[3]{};
public:
    explicit Triangle(Vector3D p[3]) {
        for (int i = 0; i < 3; i++) {
            points[i] = p[i];
        }
    }

    void draw() override;
    double intersect(const Ray& r, Color& clr, int level) override;
    Vector3D getNormal(const Vector3D &intersectionPoint) override;
};

void Triangle::draw() {
    glBegin(GL_TRIANGLES);{
        glColor3f(color.r, color.g, color.b);
        glVertex3f(points[0].x, points[0].y, points[0].z);
        glVertex3f(points[1].x, points[1].y, points[1].z);
        glVertex3f(points[2].x, points[2].y, points[2].z);
    }glEnd();
}

double Triangle::intersect(const Ray &r, Color &clr, int level) {
    Vector3D ro = r.start;
    Vector3D rd = r.dir;
    Vector3D v1 = points[0];
    Vector3D v2 = points[1];
    Vector3D v3 = points[2];

    Vector3D edge1 = v2 - v1;
    Vector3D edge2 = v3 - v1;

    Vector3D h = rd.cross(edge2);
    double a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON) {
        return -1; // ray is parallel to the triangle
    }

    double f = 1.0 / a;
    Vector3D s = ro - v1;
    double u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) {
        return -1;
    }

    Vector3D q = s.cross(edge1);
    double v = f * rd.dot(q);
    if (v < 0.0 || u+v > 1.0) {
        return -1;
    }

    double t = f * edge2.dot(q);
    if (t > EPSILON) {
        return t; // ray intersection
    } else {
        return -1; // line intersection
    }
}

Vector3D Triangle::getNormal(const Vector3D &intersectionPoint) {
    Vector3D a = points[0];
    Vector3D b = points[1];
    Vector3D c = points[2];
    Vector3D n = (b-a).cross((c-a));
    n.normalize();
    return n;
}

class GeneralQuadricSurface : public Object {
    double A, B, C, D, E, F, G, H, I, J;
public:
    GeneralQuadricSurface(double a, double b, double c, double d, double e, double f, double g, double h, double i,
                          double j, double length, double width, double height, const Vector3D& ref) : A(a), B(b), C(c), D(d), E(e), F(f), G(g), H(h), I(i), J(j) {
        this->reference_point = ref;
        this->length = length;
        this->width = width;
        this->height = height;
    }

    bool withinReferenceCube(const Vector3D& p);
    double intersect(const Ray& r, Color& clr, int level) override;
    Vector3D getNormal(const Vector3D &intersectionPoint) override;
};

bool GeneralQuadricSurface::withinReferenceCube(const Vector3D &p) {
    bool within = true;
    if (length != 0) {
        if (p.x < reference_point.x || p.x > reference_point.x + length) {
            within = false;
        }
    }
    if (width != 0) {
        if (p.y < reference_point.y || p.y > reference_point.y + width) {
            within = false;
        }
    }
    if (height != 0) {
        if (p.z < reference_point.z || p.z > reference_point.z + height) {
            within = false;
        }
    }
    return within;
}

double GeneralQuadricSurface::intersect(const Ray &r, Color &clr, int level) {
    Vector3D ro = r.start;
    Vector3D rd = r.dir;

    double a = A * rd.x * rd.x + B * rd.y * rd.y + C * rd.z * rd.z + D * rd.x * rd.y + E * rd.x * rd.z + F * rd.y * rd.z;
    double b = 2 * A * ro.x * rd.x + 2 * B * ro.y * rd.y + 2 * C * ro.z * rd.z + D * (ro.x * rd.y + ro.y * rd.x) + E * (ro.x * rd.z + ro.z * rd.x) + F * (ro.y * rd.z + ro.z * rd.y) + G * rd.x + H * rd.y + I * rd.z;
    double c = A * ro.x * ro.x + B * ro.y * ro.y + C * ro.z * ro.z + D * ro.x * ro.y + E * ro.x * ro.z + F * ro.y * ro.z + G * ro.x + H * ro.y + I * ro.z + J;

    double temp = b * b - 4 * a * c;
    if (temp < 0) return -1;
    double d = sqrt(temp);
    double t1 = (- b - d) / (2 * a);
    double t2 = (- b + d) / (2 * a);

    Vector3D p1 = ro + rd * t1;
    Vector3D p2 = ro + rd * t2;
    double t;
    if (t1 < 0 && t2 < 0) t = -1;
    else if (t1 > 0 && withinReferenceCube(p1)) t = t1;
    else if (t2 > 0 && withinReferenceCube(p2)) t = t2;
    else t = -1;
    return t;
}

Vector3D GeneralQuadricSurface::getNormal(const Vector3D &intersectionPoint) {
    double x = intersectionPoint.x;
    double y = intersectionPoint.y;
    double z = intersectionPoint.z;
    double nx = 2 * A * x + D * y + E * z + G;
    double ny = 2 * B * y + D * x + F * z + H;
    double nz = 2 * C * z + E * x + F * y + I;
    Vector3D n(nx, ny, nz);
    n.normalize();
    return n;
}


class Floor : public Object {
public:
    Floor(double floorWidth, double tileWidth) {
        reference_point = {- floorWidth / 2, - floorWidth / 2, 0};
        length = tileWidth;
    }

    void draw() override;
    double intersect(const Ray& r, Color& clr, int level) override;
    Vector3D getNormal(const Vector3D &intersectionPoint) override;
    Color getColor(const Vector3D &intersectionPoint) const override;
};

void Floor::draw() {
    glBegin(GL_QUADS);
    {
        int limit = - (int)(reference_point.x / length);
        for (int i = -limit; i < limit; i++) {
            for (int j = -limit; j < limit; j++) {
                if ((i + j) % 2 == 0) glColor3f(1, 1, 1);
                else glColor3f(0, 0, 0);
                glVertex3f(i * length, j * length, 0);
                glVertex3f(i * length + length, j * length, 0);
                glVertex3f(i * length + length, j * length + length, 0);
                glVertex3f(i * length, j * length + length, 0);
            }
        }
    }
    glEnd();
}

double Floor::intersect(const Ray &r, Color &clr, int level) {
    Vector3D n(0, 0, 1);
    Vector3D ro = r.start; // origin
    Vector3D rd = r.dir; // direction
    double t = (-1) * (n.dot(ro) / n.dot(rd));
    // check if intersection point lies within the floor
    Vector3D intersectionPoint = ro + rd * t;
    if (intersectionPoint.x < reference_point.x || intersectionPoint.x > -reference_point.x) return -1;
    if (intersectionPoint.y < reference_point.y || intersectionPoint.y > -reference_point.y) return -1;
    return t;
}

Vector3D Floor::getNormal(const Vector3D &intersectionPoint) {
    return Vector3D(0, 0, 1);
}

Color Floor::getColor(const Vector3D &intersectionPoint) const {
    if (intersectionPoint.x < reference_point.x || intersectionPoint.x > -reference_point.x) return {0, 0, 0};
    if (intersectionPoint.y < reference_point.y || intersectionPoint.y > -reference_point.y) return {0, 0, 0};

    int i = (reference_point.x + intersectionPoint.x) / length;
    int j = (reference_point.y + intersectionPoint.y) / length;

    if ((i + j) % 2 == 0) return {1, 1, 1};
    return {0, 0, 0};
}




