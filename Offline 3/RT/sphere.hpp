#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_


class Sphere : public Object {
public:
    Sphere(Vector3D center, double radius) {
        reference_point = center;
        length = radius;
    }

    void draw() override{
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        int stacks = 20, slices = 24;
        Vector3D points[100][100];
        int i,j;
        double h,r;
        double radius = length;
        //generate points
        for(i=0;i<=stacks;i++){
            h=radius*sin(((double)i/(double)stacks)*(PI/2));
            r=radius*cos(((double)i/(double)stacks)*(PI/2));
            for(j=0;j<=slices;j++){
            points[i][j].x=r*cos(((double)j/(double)slices)*2*PI);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*PI);
            points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0;i<stacks;i++){
            glColor3f(color.r,color.g,color.b);
            for(j=0;j<slices;j++){
            glBegin(GL_QUADS);

            //lower hemisphere
            glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
            glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
            glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
            glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            glEnd();


            //upper hemisphere
            glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
            glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
            glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
            glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            
            }
        }
        glTranslatef(-reference_point.x, -reference_point.y, -reference_point.z);
    }





    double intersect(const Ray& r, Color& clr, int level) override {
        Vector3D ro = r.start - reference_point;
        Vector3D rd = r.dir;
        double radius = length;
        double a = 1;
        double b = rd.dot(ro) * 2;
        double c = ro.dot(ro) - radius * radius;
        double temp = b * b - 4 * a * c;


        if (temp < 0) return -1;
        double d = sqrt(temp);
        double t1 = (- b - d) / (2 * a);
        double t2 = (- b + d) / (2 * a);

        if (t1 > 0 && t2 > 0) {
            if (t1 < t2) return t1;
            else return t2;
        } else if (t1 > 0) {
            return t1;
        } else if (t2 > 0) {
            return t2;
        } else {
            return -1;
        }

        return -1; //default, no intersection
    }



    Vector3D getNormal(const Vector3D &intersection_point) override {
        Vector3D n = intersection_point - reference_point;
        n.normalize();
        return n;
    }
};


#endif // _SPHERE_HPP_
