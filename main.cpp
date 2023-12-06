
#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <regex>
#include <set>
#include "Point.hpp"
#include "Shape.hpp"
#include "TimeExpandedNode.hpp"
#include "readxml.hpp"

using namespace std;
ofstream myfile1("Result2.txt");
std::vector<part> allpart;


// Hàm dùng Regular Expression để bắt chuỗi

int main()
{
    int sumTEN=0;
    vector<Shape *> S;
    vector<vector<TimeExpandedNode *>> allTENs;
    vector<TimeExpandedNode *> tempTENs;
    allTENs.push_back(tempTENs);
    vector<Point*> P;
    //ifstream input("/mnt/d/MinGW64/AllParts.txt");
    //input.is_open();
    int counter = 0;
    makeallpart();
    set<string> pointstr;
    map<string,vector<Shape*>> startshape;
    map<string,vector<Shape*>> endshape;
    map<string,TimeExpandedNode*> mapTEN;
    set<string> shapestr;
    for(auto& a : allpart)
    {
        Shape *shape = new Shape();
        //shape->setName(a.name);
        Point *firstPoint = new Point();
        a.Point1.first = std::round(a.Point1.first * 100.0)/ 100.0;
        a.Point1.second = std::round(a.Point1.second * 100.0)/ 100.0;
        firstPoint->setPoint(a.Point1.first,a.Point1.second);
        Point *lastPoint = new Point();
        a.Point2.first = std::round(a.Point2.first * 100.0)/ 100.0;
        a.Point2.second =  std::round(a.Point2.second * 100.0)/ 100.0;
        lastPoint->setPoint(a.Point2.first,a.Point2.second);
        if(!firstPoint->equals(lastPoint)){
          shape->setPoint(firstPoint, lastPoint);
          shape->setName(a.name);
          string coord1 = to_string(firstPoint->x)+" "+to_string(firstPoint->y);
          string coord2 = to_string(lastPoint->x)+" "+to_string(lastPoint->y);
          
          if (pointstr.find(coord1)==pointstr.end()){
            P.push_back(firstPoint);
            pointstr.insert(coord1);
            TimeExpandedNode *n1 = new TimeExpandedNode();
            n1->setTENode(firstPoint);
            allTENs.at(0).push_back(n1);
          }
          if (pointstr.find(coord2)==pointstr.end()){
            P.push_back(lastPoint);
            pointstr.insert(coord2);
            TimeExpandedNode *n1 = new TimeExpandedNode();
            n1->setTENode(lastPoint);
            allTENs.at(0).push_back(n1);
          }
          string shapecoord = coord1+"_"+coord2;
          if (shapestr.find(shapecoord)==shapestr.end()){
            S.push_back(shape);
          }
        }
    }
    for(auto& s : S){
      string firstcoord = to_string(s->start->x)+" "+to_string(s->start->y);
      string lastcoord = to_string(s->end->x)+" "+to_string(s->end->y);
      if (pointstr.find(firstcoord)!=pointstr.end()){
        startshape[firstcoord].push_back(s);
      }
      if(pointstr.find(lastcoord)!=pointstr.end()){
        endshape[lastcoord].push_back(s);
      }
    }
    
    for (TimeExpandedNode *n : allTENs.at(0))
    {
        string coord = to_string(n->origin->x)+" "+to_string(n->origin->y);
        auto it  = startshape.find(coord);
        if (it!=startshape.end()){
          for (auto& a : it->second){
            n->insertTarget(a);
          }
        }
        auto it1 = endshape.find(coord);
        if (it1!=endshape.end()){
          for (auto& a : it1->second){
            n->insertSource(a);
          }
        }
        mapTEN[coord] = n;
    }
    for (TimeExpandedNode *n : allTENs.at(0))
    {
      for (auto& a : n->srcs){
        string coord = to_string(a.second->start->x)+" "+to_string(a.second->start->y);
        auto it = mapTEN.find(coord);
        if (it!=mapTEN.end()){
          a.first=it->second;
        }
      }
      for (auto& a : n->tgts){
        string coord = to_string(a.second->end->x)+" "+to_string(a.second->end->y);
        auto it = mapTEN.find(coord);
        if (it!=mapTEN.end()){
          a.first=it->second;
        }
      }
      
    }
    
    //
    /*for(auto& it : allpart){
      int count = 0;
      for (auto& a:allTENs.at(0))
      {
        for (auto& src : a->srcs)
        {
          if (it.name == src.second->name)
          {
            count++;
          }
        }
        for (auto& tgts: a->tgts)
        {
           if (it.name == tgts.second->name)
           {
            count++;
           }
           
        }
        if (count == 1)
        {
          for (auto& src : a->srcs)
          {
           if (it.name == src.second->name)
           {
             src.second->time = (3.9/4 + 3.9*3/4)/(2*count);
           }
          }
          for (auto& tgts: a->tgts){
            if (it.name == tgts.second->name)
            {
             tgts.second->time = (3.9/4 + 3.9*3/4)/(2*count);
            }
          } 
        }
        if (count == 2)
        {
          for (auto& src : a->srcs)
          {
           if (it.name == src.second->name)
           {
             src.second->time = (3.9/4 + 3.9/4*3+3.9*2/4)/(4*count);
           }
          }
          for (auto& tgts: a->tgts){
            if (it.name == tgts.second->name)
            {
             tgts.second->time = (3.9/4 + 3.9*3/4+2*3.9*2/4)/(4*count);
            }
          } 
        }
      }
      
    }*/
    map<string,int> checkpointdup;
    for (auto &pair : checkpointdup) {
        pair.second = 0;
    }
    for (TimeExpandedNode *n : allTENs.at(0)){
      string coord =  to_string(n->origin->x)+" "+to_string(n->origin->y);
      assert(n->time == 0);
      checkpointdup[coord]++;
    }
    for (auto &pair : checkpointdup) {
      assert(pair.second == 1);
    }
    assert(checkpointdup.size()==P.size());
    map<string,int> checkshapedup;
    for (auto &pair : checkshapedup) {
        pair.second = 0;
    }
    for (TimeExpandedNode *n : allTENs.at(0)){
      for (pair<TimeExpandedNode *, Shape *> x : n->srcs){
         assert(x.first != nullptr && x.second != nullptr);
         string coord1 = to_string(x.second->start->x)+" "+to_string(x.second->start->y);
         string coord2 = to_string(x.second->end->x)+" "+to_string(x.second->end->y);
         string coord = coord1 + coord2;
         checkshapedup[coord]++;
      }
      for (pair<TimeExpandedNode *, Shape *> x : n->tgts){
        if(x.first == nullptr || x.second == nullptr){
			   cout<<"DEBUYG"<<endl;
		    }
        assert(x.first != nullptr && x.second != nullptr);
        string coord1 = to_string(x.second->start->x)+" "+to_string(x.second->start->y);
        string coord2 = to_string(x.second->end->x)+" "+to_string(x.second->end->y);
        string coord = coord1 + coord2;
        checkshapedup[coord]++;        
      }
    }
    for (auto &pair : checkshapedup) {
        assert(pair.second == 2);
    }
    assert(checkshapedup.size()==S.size());
    for (TimeExpandedNode *n : allTENs.at(0))
    {
        int sum = n->srcs.size() + n->tgts.size();
        int i = 0;
        for (TimeExpandedNode *m : allTENs.at(0))
        {
            for (pair<TimeExpandedNode *, Shape *> x : m->srcs)
            {
                if (n == x.first)
                    i++;
            }

            for (pair<TimeExpandedNode *, Shape *> x : m->tgts)
            {
                if (n == x.first)
                    i++;
            }
        }
       
        assert(sum == i);
        // Nếu phần tử n có tổng độ dài của srcs và tgts của nó là X. Thì số lần xuất hiện của n trong các vector
        // srcs và tgts của các TENodes khác cũng sẽ bằng đúng X
    }
    
    //cout<<"End of fifth nested-for loop"<<endl;
    //cout<<"DONE."<<endl;
    int count = 0;
    vector<int>  initializations = getStartedNodes(allTENs);
    for(int index : initializations){
		  spread(allTENs, 0, index,50);
    }

    //cout<<allTENs.size()<<endl;
    //cout<<newNode->tgts.size()<<endl;
    std::vector<std::pair<int, int>> redundants = filter(allTENs);
    remove2(redundants,allTENs);
    //cout<<redundants.size()<<endl;
    for(auto& it: allTENs)
    {
     assertTime(it,1);
    }
    for(auto& it : allTENs)
    {
     sumTEN = sumTEN+it.size();
    }
    cout<<sumTEN<<endl;
    cout<<"-------------------------------------------------------"<<endl;
   
    connectAllChains(allTENs,50,1);
     for(auto& it : allTENs)
    {
     sumTEN = sumTEN+it.size();
    }
    cout<<sumTEN<<endl;
    //connectAllChains(allTENs,Points,2);
    //(allTENs,P,90);
    cout<<"-------------------------------------------------------"<<endl;
    /*std:: string filename = "intinerary.txt";
    std::string station="";
    std::map<std::string, std::vector<ArtificialStation*>> mapArtificialStations = getTimeWindows(filename,2,station);
    replaceStation(allTENs,station);
    int i,k;
    for( i = 0; i < allTENs.size( ) ; i++){
	auto& v = allTENs.at(i);
	for(int j = 0; j < v.size( ); j++){
		auto& temp = v.at(j);
		if(instanceof<Station>(temp)){
			std::string name = temp->name;
			auto foundit = mapArtificialStations.find(name);
			if(foundit != mapArtificialStations.end()){
				auto& allArStations = mapArtificialStations[name];
				for(auto& elem : allArStations){
					elem->createConnection(temp);
				}
			}
		}
	 }
    }
    for (const auto& el : mapArtificialStations){ 
     auto& v = el.second;
	 for(auto& elem : v){
		insert(allTENs, elem);
	 }
    }
    redundants = filter(allTENs);
    remove2(redundants,allTENs);
    
    for(auto& it: allTENs)
    {
     assertTime(it,1);
    }*/
    /*sumTEN = 0;
    for(auto& it : allTENs)
    {
    
     sumTEN = sumTEN+it.size();
    }
    cout<<sumTEN<<endl;
    
    int autoIncreament = 0;
    vector<TimeExpandedNode*> starnode;
    for(auto& it : allTENs)
    {
        for(auto& a: it)
        {
          if(a->srcs.empty())
          {
            starnode.push_back(a);
          }
        }
    }
    for(auto& element : starnode){
	 assignKey(element,&autoIncreament);
    }
    cout<< COUNTER<<endl;*/
    //writeFile(allTENs,sumTEN);
    
    //assert(checkautoincreament(autoIncreament,sumTEN));
    //assert(checkduplicate(allTENs,sumTEN));*/
}