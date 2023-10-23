#include "readxml.hpp"
using namespace std;
typedef std::pair<double, double> Point;
typedef struct{
   std::pair<double, double> Point1;
   std::pair<double, double> Point2;
   double value;
}shape;
typedef struct{
   std::string id;
   shape shapeofedge;
}edge;
typedef struct{
  std::string id;
  std::vector<std::pair<double, double>> points;
}junc;
vector<std::pair<Point, Point>> listPoints(Point A, Point B, double x)
{
    double d = sqrt(pow(B.first - A.first, 2) + pow(B.second - A.second, 2));
    int n = int(d / x);
    float x1, y1, x2, y2;
    vector<std::pair<Point, Point>> list;
    double ratio = x / d;
    if (n == 0)
    {
        list.push_back({A, B});
        return list;
    }

    for (int i = 0; i < n; i++)
    {
        x1 = A.first + i * (B.first - A.first) * ratio;
        y1 = A.second + i * (B.second - A.second) * ratio;
        x2 = A.first + (i + 1) * (B.first - A.first) * ratio;
        y2 = A.second + (i + 1) * (B.second - A.second) * ratio;
        list.push_back({Point({x1, y1}), Point({x2, y2})});
    }
    if (d / x != int(d / x))
    {
        list.push_back({Point({x2, y2}), B});
    }

    return list;
}
void splitEdge(double x, edge *e)
{
    vector<std::pair<Point, Point>> list = listPoints(e->shapeofedge.Point1,e->shapeofedge.Point2, x);
    int i = 0;
    for (std::pair<Point, Point> point : list)
    {
        //myfile << e->id << " " << i << "_" << fixed << setprecision(2) << point.first.first << "," << point.first.second << "_" << point.second.first << "," << point.second.second << "\n";
        part a;
        a.name = e->id;
        a.i=i;
        a.Point1 = point.first;
        a.Point2 = point.second;
        allpart.push_back(a);
        i++;
    }
}

void splitJunc(double x, junc *j)
{
    vector<Point> points = j->points;
    int n = 0;
    for (int i = 0; i < points.size() - 1; i++)
    {
        vector<std::pair<Point, Point>> list = listPoints(points[i], points[i + 1], x);
        for (std::pair<Point, Point> point : list)
        {
            //myfile << j->id << " " << n << "_" << point.first.first << "," << point.first.second << "_" << point.second.first << "," << point.second.second << "\n";
            part a;
            a.name = j->id;
            a.i = n;
            a.Point1 = point.first;
            a.Point2= point.second;
            allpart.push_back(a);
            n++;
        }
    }
}
void makeallpart(){
    vector<edge> edges;
    vector<junc> juncs;
    vector<edge> starts;
    vector<edge> ends;
    vector<string> dead_end;
    vector<edge> newedges;
    pugi::xml_document doc;
    //myfile.open("AllParts.txt");
    if (!doc.load_file("/mnt/d/MinGW64/lab2/net.net.xml"))
    {
        cout << "Can't read" << endl;
    }
    pugi::xml_node panels = doc.child("net");
    pugi::xml_attribute attr;
    for (pugi::xml_node panel = panels.child("junction"); panel; panel = panel.next_sibling("junction"))
    {
        attr = panel.first_attribute();
        if (strcmp(attr.next_attribute().name(), "type") == 0 && strcmp(attr.next_attribute().value(), "dead_end") == 0)
        {
            dead_end.push_back(attr.value());
            //cout<<attr.value()<<endl;
        }
    }
    for (pugi::xml_node panel = panels.child("edge"); panel; panel = panel.next_sibling("edge")){
        attr = panel.first_attribute();
         if (strcmp(attr.next_attribute().name(), "from") == 0 && strcmp(attr.next_attribute().next_attribute().name(), "to") == 0 && strcmp(attr.next_attribute().value(), attr.next_attribute().next_attribute().value()) != 0)
         {
           int check1 = 0;
            int check2 = 0;
            for (string s : dead_end)
            {
                if (strcmp(attr.next_attribute().value(), s.c_str()) == 0)
                {
                    //cout<<attr.next_attribute().value()<<endl;
                    check1 = 1;
                }
                if (strcmp(attr.next_attribute().next_attribute().value(), s.c_str()) == 0)
                {
                    //cout<<attr.next_attribute().value()<<endl;
                    check2 = 1;
                }
                if (check1 == 1 && check2 == 1)
                {
                    break;
                }
            }
          //cout<<attr.value()<<endl;  
          edge e;
          string shape;
          double length;
          int i = 0;
          e.id = attr.value();
          for (pugi::xml_node child = panel.first_child(); child; child = child.next_sibling())
            {
                attr = child.first_attribute().next_attribute().next_attribute();
                if (strcmp(attr.name(), "disallow") == 0 && strcmp(attr.value(), "pedestrian") == 0)
                {
                    shape = attr.next_attribute().next_attribute().next_attribute().value();
                    length = atof(attr.next_attribute().next_attribute().value());
                    i++;
                    //cout<<shape<<" "<<length<<endl;
                }
                if (i > 1)
                    break;
            }
            if(i == 1)
            {
                vector<double> nums;
                stringstream ss(shape);
                double num;
                while (ss >> num)
                {
                    nums.push_back(num);
                    if (ss.peek() == ',' || ss.peek() == ' ')
                    {
                        ss.ignore();
                    }
                }
                e.shapeofedge.Point1={nums[0], nums[1]};
                e.shapeofedge.Point2={nums[2], nums[3]};
                e.shapeofedge.value= length;
                if (check1 == 1)
                {
                    starts.push_back(e);
                }
                if (check2 == 1)
                {
                    ends.push_back(e);
                }

                edges.push_back(e);
            }
             
         }
         else if (strcmp(attr.next_attribute().name(), "function") == 0 && strcmp(attr.next_attribute().value(), "internal") == 0){
            junc j;
            j.id = attr.value();
            string shape;
            double length;
            int i = 0;
            for (pugi::xml_node child = panel.first_child(); child; child = child.next_sibling())
            {
                attr = child.first_attribute().next_attribute().next_attribute();
                if (strcmp(attr.name(), "disallow") == 0 && strcmp(attr.value(), "pedestrian") == 0)
                {
                    shape = attr.next_attribute().next_attribute().next_attribute().value();
                    length = atof(attr.next_attribute().next_attribute().value());
                    i++;
                }
                if (i > 1)
                    break;
            }
            if(i==1)
            {
                vector<double> nums;
                stringstream ss(shape);
                double num;
                while (ss >> num)
                {
                    nums.push_back(num);
                    if (ss.peek() == ',' || ss.peek() == ' ')
                    {
                        ss.ignore();
                    }
                }

                std::vector<Point> points;

                for (int i = 0; i < nums.size() - 1; i += 2)
                {
                    points.push_back(Point({nums[i], nums[i + 1]}));
                }
                j.points = points;
                juncs.push_back(j);
            }
         }
    }
    for (edge e : edges)
    {
        splitEdge(3.9, &e);
    }
    edges.clear();
    for (junc j : juncs)
    {
        splitJunc(3.9, &j);
    }
    juncs.clear();

}