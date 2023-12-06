#include <iostream>
#include <string>
#include <string.h>
#include <assert.h>
#include <cmath>
#include "Point.hpp"
#include "Shape.hpp"
#include "readfile.h"
#include <vector>
#include <cmath>
#include <utility>
#include <set>
#include <queue>
using namespace std;
int *x;

int COUNTER = 0;

ofstream myfile("Result.txt");
typedef struct{
    int pos;
    int min;
    int max;
}position;
template<typename Base, typename T> 
inline bool instanceof(const T *ptr) 
{ return dynamic_cast<const Base*>(ptr) != nullptr; }
class TimeExpandedNode
{
public:
    vector<pair<TimeExpandedNode *, Shape *>> srcs;
    Point *origin;
    double time;
    int key = -1;
    string name;
    int layer;
    vector<pair<TimeExpandedNode *, Shape *>> tgts;

    void setTENode(Point *point)
    {
        this->origin = point;
    }

    void insertSource(Shape *s)
    {
        // Nếu srcs có chứa một pair mà phần tử đầu của nó giống hệt s->start
        //     thì không thêm nullptr và s vào
        //         Ngược lại
        {
            pair<TimeExpandedNode *, Shape *> pair =
                make_pair(nullptr, s);
            this->srcs.push_back(pair);
        }
    }

    void insertTarget(Shape *s)
    {
        // Nếu tgts có chứa một pair mà phần tử đầu của nó giống hệt s->end
        //     thì không thêm cặp nullptr và s vào
        //         Ngược lại
        {
            pair<TimeExpandedNode *, Shape *> pair =
                make_pair(nullptr, s);
            this->tgts.push_back(pair);
        }
    }

    bool equals(TimeExpandedNode *n)
    {
        return this->origin->equals(n->origin);
    }

    void insertSourcesAndTargets(TimeExpandedNode *n, vector<pair<int, int>> fromN,
                                 vector<pair<int, int>> toN)
    {
        for (int i = 0; i < fromN.size(); i++)
        {
            int atN = fromN.at(i).first;
            int toThis = fromN.at(i).second;
            if (this->srcs.at(atN).first == nullptr)
            {
                this->srcs.at(atN).first = n;
            }
             
            if (n->tgts.at(toThis).first == nullptr)
            {
                n->tgts.at(toThis).first = this;
            }
        }

        for (int i = 0; i < toN.size(); i++)
        {
            int atN = toN.at(i).first;
            int fromThis = toN.at(i).second;
            if (this->tgts.at(fromThis).first == nullptr)
            {
                this->tgts.at(fromThis).first = n;
            }
            if (n->srcs.at(atN).first == nullptr)
            {
                n->srcs.at(atN).first = this;
            }
        }
    }
    void ExpandedNode (Point* origin, double time, 
                           std::vector<std::pair<TimeExpandedNode*, Shape*>> tgts){
                        this->origin = origin;
                        this->time = time;
                        for(auto& it : tgts){
                                  this->tgts.push_back(std::make_pair(nullptr, it.second));
                        }                        
	}


    int indexInSources(Shape* s){
           int index = 0;
           bool found = false;
           for(auto& it : srcs){
                      Shape* shape = it.second;
                      if(shape->equals(s)){
                                 found = true;
                                 return index;
                      }
                      index++;
           }
           if(!found)   return -1;
           return index;             
    }
    bool endOfLane( ){
	if(this->tgts.size() <= 1) return true;
	for(int i = 0; i < this->srcs.size( ); i++){
		auto shape = this->srcs.at(i).second;
		auto name = shape->name;
		if(name.compare("artificial") != 0){
			for(int j = 0; j < this->tgts.size( ); j++){
				auto& nextShape = this->tgts.at(j).second;
				auto nextName = nextShape->name;
				if(nextName.compare("artificial") != 0){
					if(nextName.compare(name) != 0){
						return true;
					}
				}
			}
		}
	 }
	 return false;
    }
    std::string isStation(std::string stations){
        for(int i = 0; i < this->srcs.size( ); i++){
	    auto& shape = this->srcs.at(i).second;
	    auto name = shape->name;
	    if(name.compare("artificial") != 0){
		if(stations.find("$" + name + "$") != std::string::npos){
			return name;
		}
	 }	
    }
    return "";
    }
    virtual void createConnection(TimeExpandedNode* node){}
};
class Station :  public TimeExpandedNode{
    public:
    void makestation(TimeExpandedNode* temp, std::string name)
    {
        this->srcs = temp->srcs;
        this->name = name;
        this->tgts = temp->tgts;
        this->origin = temp->origin;
        this->time = temp->time;
        for(auto& pr : this->srcs){
                 for(auto& pt : pr.first->tgts){
                        if(pt.first == temp){ 
                                 pt.first = this;
                        }
                 }
         }
         for(auto& pr : this->tgts){
                 for(auto& pt : pr.first->srcs){
                        if(pt.first == temp){ 
                                 pt.first = this;
                        }
                 }
        }

    }
};
class PausingShape : public Shape
{
public:
    PausingShape( ){ d = 0; name = "artificial"; time = 0;}
    PausingShape(double time) {
        this->time = time;
                name = "artificial"; 
    }
    PausingShape(Shape *s, std::string name){
        this->name = name;
        this->start = s->start;
        this->end = s->end;
        this->d = s->d;
        this->time = s->getWeight();
    }
    //Phương thức getTime được ghi đè như sau
    double getTime( ){
        return this->time;
    }

};
class ArtificialShape : public PausingShape{
 public:
  ArtificialShape(double time) : PausingShape(time) {}
 //Phương thức getTime được ghi đè như sau
 double getWeight( ) override {
            return this->time;
  }
};
class ArtificialStation : public Station{
    public:
    double bestTime;
    double amplitude;
    double earliness, tardiness;
    ArtificialStation(std::string name, double bestTime, double amplitude) : Station( ){
         this->name = name;
         this->time = bestTime;
         this->amplitude = amplitude;
         this->earliness = bestTime - amplitude;
         this->tardiness = bestTime + amplitude;
    }
    void createConnection(TimeExpandedNode* node) override{
	if(instanceof<Station>(node) && !instanceof<ArtificialStation>(node)){
		if(node->name == this->name){
			//để gọi được hàm max cần dùng thư viện cmath
			auto penaltyT = max(this->earliness - node->time, 0.0 );
            penaltyT = max(penaltyT, node->time - this->tardiness); 
			auto aShape = new ArtificialShape(penaltyT);
			node->tgts.push_back(std::make_pair(this, aShape));
			this->srcs.push_back(std::make_pair(node, aShape));
		}
	 }
    }


};

TimeExpandedNode* isAvailable(std::vector<std::vector<TimeExpandedNode*>> graph, Point* origin, double time){
    for(auto& vec : graph){
      if (vec.at(0)->time == time){
        for (auto& it : vec){
          if(it->origin->equals(origin)) return it;
        }
      }
    }
    return nullptr;
     /*for(auto& vec : graph)
     {
      for(auto& it : vec){
        if(it->origin->equals(origin)&& it->time==time)
        {
            return it;
        }
      }
     }
     return nullptr;*/
}
bool isavailable(std::vector<std::vector<TimeExpandedNode*>> graph, double time, int index){
   int count = 0;
   for(auto& it : graph.at(index))
   {
    if(it->time == time) count++;
   }     
   if(count == graph.at(index).size()) {return true;}
   else{
       index = index + 1;
       return false;
   }
}
void insert(std::vector<std::vector<TimeExpandedNode*>> &graph, TimeExpandedNode *ten)
{
     int count = 0;
     for(auto& it :graph)
     {
       if (it.at(0)->time == ten->time)
        {
            it.push_back(ten);
            return;
        }
     }
    vector<TimeExpandedNode *> tempTEN;
    tempTEN.push_back(ten);
    graph.push_back(tempTEN);
     
} 
void spread(std::vector<std::vector<TimeExpandedNode*>> &graph, int m, int n, double H) {
    map<string,int> checkTENdup;
    for (auto &pair : checkTENdup) {
        pair.second = 0;
    }
    TimeExpandedNode* node = graph[m][n];
    std::queue<TimeExpandedNode*> Q;
    Q.push(node);
    
    while (!Q.empty()) {
        TimeExpandedNode* temp = Q.front();
        Q.pop();
        
        for (auto& pair : temp->tgts) {
            string coord = to_string(pair.first->origin->x)+" "+to_string(pair.first->origin->y);
            if (checkTENdup[coord]!=1){
                checkTENdup[coord]=1;
                Shape* s = pair.second;
                double time = temp->time + s->getWeight();
                if (time < H) {
                Point* origin = s->end;  TimeExpandedNode* n = pair.first;
                TimeExpandedNode* foundItem = isAvailable(graph, origin, time);
                if (foundItem == nullptr) {
                    TimeExpandedNode* newNode = new TimeExpandedNode();
                    newNode->setTENode(origin);
                    newNode->time= time;
                    for(auto& it : n->tgts){
                      newNode->tgts.push_back(make_pair(nullptr,it.second));
                    }
                    newNode->tgts = n->tgts;
                    pair.first = newNode;
                    foundItem = newNode;
                 }
                 int index = foundItem->indexInSources(s);
                 if (index != -1) {
                    foundItem->srcs[index].first = temp;
                 }
                 else {
                    foundItem->srcs.push_back(std::make_pair(temp, s));
                 }
                 insert(graph, foundItem); // Gọi hàm đã làm ở câu (d)
                 Q.push(foundItem);
                }
            }
           
        }
    }
    map<string, int>().swap(checkTENdup);
}

std::vector<std::pair<int, int>> filter(std::vector<std::vector<TimeExpandedNode*>> &graph){
    vector<std::pair<int,int>> a;
    int i,k;
    for(i = 0;i<graph.size();i++)
    {
        for(k=0;k<graph[i].size();k++)
        {
           int count  = 0;
           for(auto& it : graph.at(i).at(k)->srcs)
           {
            if(it.first->time+it.second->getWeight() > graph.at(i).at(k)->time){
              count++;
              //a.push_back(make_pair(i,k));
            }
            
           }
           if(count > 0)
            {
             a.push_back(make_pair(i,k));
            }  
        }
    }
    return a;
}  
void remove(vector<pair<int, int>> filters,
            vector<vector<TimeExpandedNode *>> &graph)
{
    int length = filters.size();
    for (int i = length - 1; i >= 0; i--)
    {
        pair<int, int> p = filters.at(i);
        // cout << graph.at(0).at(p.second)->origin->x << " - "
        //      << graph.at(0).at(p.second)->origin->y << " --- " << p.second << endl;

        graph.at(0).erase(graph.at(0).begin() + p.second);
    }
}
void remove2(vector<pair<int, int>> filters,
            vector<vector<TimeExpandedNode *>> &graph){
        int length = filters.size();
        for (int i = length - 1; i >= 0; i--){
            pair<int, int> p = filters.at(i);
            graph.at(p.first).erase(graph.at(p.first).begin()+p.second);
        }
}
std::vector<int> getStartedNodes(std::vector<std::vector<TimeExpandedNode*>> graph)
{
    std::vector<int> a;
    int k ;
    for(auto& it : graph)
    {
        for(k = 0;k<it.size();k++)
        {
            if(it[k]->srcs.empty())
            {
                if(it[k]->time==0)
                {
                    a.push_back(k);
                }
            }
        }
    }
    return a;
}
void assertTime(std::vector<TimeExpandedNode*> graph, double v){
    int stage  = 0;
    for(auto& it : graph)
    {
        int count = 0;
        for(auto& a : it->srcs)
        {
            if(a.first->time + a.second->getWeight() !=  it->time) count++;
        }
        if(count != 0) { //cout<<count<<endl;
            stage =1;}
    }
    if(stage == 1) cout<<"Error assertTime"<<endl;
}
// code bai 4 dang can hoan thien 
std::map<std::string, std::vector<TimeExpandedNode*>> getchains(std::vector<std::vector<TimeExpandedNode*>> graph) {
    std::map<std::string, std::vector<TimeExpandedNode*>> result;
    for (auto& it : graph) {
        for (auto& TEN : it) {
            std::string coord = std::to_string(TEN->origin->x) + " " + std::to_string(TEN->origin->y);
            result[coord].push_back(TEN); // Corrected 'pushback' to 'push_back'
        }
    }
    return result;
}
std::vector<std::pair<double, TimeExpandedNode*>> makenewchains(std::vector<TimeExpandedNode*> chain, double H, double delta) {
    std::vector<std::pair<double, TimeExpandedNode*>> result;

    int i = 0;
    auto& root = chain.at(0);
    while (i < chain.size() - 1) {
        auto& pre = chain.at(i);
        auto& next = chain.at(i + 1);
        int steps = floor((next->time - pre->time) / delta);

        for (int k = 0; k < steps; ++k) {
            TimeExpandedNode* newNode = new TimeExpandedNode();
            newNode->setTENode(root->origin);
            newNode->time = pre->time + delta * (k + 1);
            //newNode->tgts = pre->tgts;
            for(auto& it : root->tgts){
                      newNode->tgts.push_back(make_pair(nullptr,it.second));
            }
            result.push_back(std::make_pair(newNode->time, newNode));
        }
        ++i;
    }

    auto& last = chain.back(); // Accessing the last element of 'chain'
    int steps = floor((H - last->time) / delta);
    for (int j = 0; j < steps; ++j) {
        TimeExpandedNode* newNode = new TimeExpandedNode();
        newNode->setTENode(root->origin);
        newNode->time = last->time + delta * (j + 1);
        //newNode->tgts = last->tgts;
        for(auto& it : root->tgts){
            newNode->tgts.push_back(make_pair(nullptr,it.second));
        }
        result.push_back(std::make_pair(newNode->time, newNode));
    }

    return result;
}

std::vector<std::pair<int, int>> insert2(std::vector<std::vector<TimeExpandedNode*>> &graph,std::vector<std::pair<double,TimeExpandedNode*>> newchains){
 std::vector<std::pair<int, int>> result;
 int i,k;
 for (auto& newchain : newchains){
  int size = graph.size();
  if (newchain.first > graph[size-1].at(0)->time)
  {
    std::vector<TimeExpandedNode*> newTEN;
    newTEN.push_back(newchain.second);
    graph.push_back(newTEN);
    result.push_back(make_pair(graph.size()-1,0));
  }
  else{
    int count = 0;
    for (i = 0;i<size;i++){
     if (newchain.first==graph.at(i).at(0)->time){
      graph.at(i).push_back(newchain.second);
      result.push_back(make_pair(i,graph.at(i).size()-1));
      count++;
     }
    }
    if (count == 0){
      i = 0;
      while(0==0){
        if(newchain.first<graph.at(i).at(0)->time){
          break;
        }
        i++;
      }
      vector<TimeExpandedNode*> newTEN;
      graph.insert(graph.begin()+i,1,newTEN);
    }
    
  }
 }
 return result;
} 
bool checkInsertion(std::vector<std::vector<TimeExpandedNode*>> graph, 
	std::vector<std::pair<double,TimeExpandedNode*>> newChains){
	for(auto& elem : newChains){
		if(isAvailable(graph,elem.second->origin, elem.first)!=nullptr){
			return true;
		}
	}
	return false;
}
bool checkresult(std::vector<std::vector<TimeExpandedNode*>> graph,std::vector<std::pair<int, int>> newpos,
std::vector<std::pair<double,TimeExpandedNode*>> newChains){
  int i;
  for ( i = 0; i < newpos.size() ; i++)
  {
    if (!graph.at(newpos.at(i).first).at(newpos.at(i).second)->origin->equals(newChains.at(i).second->origin)||
    graph.at(newpos.at(i).first).at(newpos.at(i).second)->time!=newChains.at(i).first)
    {
      return false;
    }
  }
  return true;
}
void spread2(std::vector<std::vector<TimeExpandedNode*>> &graph, int m, int n, double H){
    TimeExpandedNode* node = graph[m][n];
    std::queue<TimeExpandedNode*> Q;
    Q.push(node);
    while (!Q.empty()){
        TimeExpandedNode* temp = Q.front();
        Q.pop();
        for (auto& pair : temp->tgts) {
        Shape* s = pair.second;
        double time = temp->time + s->getWeight();
        if (time < H) {
            Point* origin = s->end;  TimeExpandedNode* n = pair.first;
            TimeExpandedNode* foundItem = isAvailable(graph, origin, time);
            if (foundItem == nullptr) {
                TimeExpandedNode* newNode = new TimeExpandedNode();
                newNode->setTENode(origin);
                newNode->time= time;
                if (n!= nullptr)
                {
                 for(auto& it : n->tgts){
                    newNode->tgts.push_back(make_pair(nullptr,it.second));
                 }
                }
                pair.first = newNode;
                foundItem = newNode;
                }
                int index = foundItem->indexInSources(s);
                if (index != -1) {
                    foundItem->srcs[index].first = temp;
                }
                else {
                    foundItem->srcs.push_back(std::make_pair(temp, s));
                }
                insert(graph, foundItem); // Gọi hàm đã làm ở câu (d)
                Q.push(foundItem);
            }
        }
        
    }
}

/*void assertNewOrder(std::vector<std::pair<int, int>> newOrder, std::vector<std::vector<TimeExpandedNode*>> graph)
{
    for(auto& e1 : newOrder){
	   for(auto& e2 : newOrder){
		if(e1.first < e2.first){			
           assert(graph.at(e1.first).at(e1.second)->time < graph.at(e2.first).at(e2.second)->time);
		}
	  }
    }
}*/


/*std::vector<std::vector<TimeExpandedNode*>> connectChains(
std::vector<std::vector<TimeExpandedNode*>> &graph,
					std::vector<std::pair<int, int>> newOrder)
{
    for(int i = 0; i < newOrder.size() - 1; i++){
        int i1Prev = newOrder.at(i).first;
        int i2Prev = newOrder.at(i).second;
        int j1Next = newOrder.at(i+1).first;
        int j2Next = newOrder.at(i+1).second;
        TimeExpandedNode* prev = graph.at(i1Prev).at(i2Prev);
        TimeExpandedNode* next = graph.at(j1Next).at(j2Next);
        auto s0 = new PausingShape(next->time - prev->time);
        s0->start = prev->origin; 
        s0->end = next->origin;
        prev->tgts.push_back(std::make_pair(next, s0));
        next->srcs.push_back(std::make_pair(prev, s0));
    }
    return graph;

}*/
void connectAllChains(std::vector<std::vector<TimeExpandedNode*>> &graph,double H,double delta)
{
    map<string,vector<TimeExpandedNode*>> oldchain = getchains(graph);
    for (auto& pair : oldchain){
      std::vector<std::pair<double,TimeExpandedNode*>> newchains = makenewchains(pair.second,H,delta);
      vector<std::pair<int, int>> newpos = insert2(graph,newchains);
      assert(checkInsertion(graph,newchains));
      assert(checkresult(graph,newpos,newchains));
      for (auto& pos : newpos){
       spread2(graph,pos.first,pos.second,50);
      }
    }
}
void replaceStation (std::vector<std::vector<TimeExpandedNode*>> &graph,std::string stations){
         for(int i = 0; i < graph.size(); i++){
                  auto& v = graph.at(i);
                  for(int j = 0; j < v.size(); j++){
                           auto& temp = v.at(j);
                           if(temp->endOfLane()){
                                    auto name = temp->isStation(stations);
                                    if(name!=""){
                                            Station *tmp = new Station();
                                            tmp->makestation(temp,name);
                                            graph.at(i).at(j) = tmp;
                                    }    
                           }
                  }
        } 
         
}
std::map<std::string, std::vector<ArtificialStation*>> getTimeWindows(std::string fileName, double H, std::string &stations){
    std::map<std::string, std::vector<ArtificialStation*>> result; 
    FILE* f =fopen(fileName.c_str(),"r");
    char line[50];
    int i;
    while (!feof(f))
    {
        fscanf(f,"%[^\n]%*c",line);
        int besttime =getBestTime(line);
        int Amplitude = getAmplitude(line);
        char name[10];
        getName(line,name);
        std::vector<ArtificialStation*> values ;
        int period=getPeriod(line);
        for(int i = 0; i < H; i += period){
		 values.push_back(new ArtificialStation(name, besttime + i, Amplitude));
	    }
        std::string namestr(name);
        if(values.size( ) > 0){
            result.insert({namestr, values});
            stations.append("$" + namestr + "$");
        }
        
    }
    fclose(f);
    return result;
}
void assignKey(TimeExpandedNode* element, int* autoIncreament){
    
    if(element == nullptr)
	  return;
    if(element->key == -1){
        //element->key = *autoIncreament;
        //*autoIncreament = *autoIncreament + 1;
        element->key = COUNTER;
        COUNTER++;
    }
    if(element->tgts.empty()){
      return;         
    }
    for(auto& e : element->tgts){
	 assignKey(e.first, autoIncreament);
    }
}
bool checkautoincreament(int autoincreament,int sumten)
{
    if((autoincreament - sumten)==1)
    {
        return true;
    }
    return false;
}
bool checkduplicate(std::vector<std::vector<TimeExpandedNode*>> graph,int sumten){
  x = new int[sumten];
  memset(x,0,sizeof(int) * sumten);
  int i;
  int count = 0;
  for(auto& it : graph){
    for(auto& a : it){
        if(x[a->key]==0){
           x[a->key] = 1;
        }
        else
        {
            cout<<a->key<<endl;
        }
        
    }
  }
  /*for ( i = 0; i < sumten; i++){
    if(x[i]!=1){
        return false;
        //count++;
    } 
  }*/
  return true;
}
void writeFile(std::vector<std::vector<TimeExpandedNode*>> graph,int sumten)
{
 int sumedge = 0;
 for(auto& it : graph)
 {
   for(auto& a : it){
     sumedge = a->srcs.size()+sumedge+a->tgts.size();
   }
 }
 myfile<<"p"<<" "<<"min"<<" "<<sumten<<" "<<sumedge<<endl;
 for(auto& it : graph){
    for(auto& a : it){
        for (auto& b : a->srcs){
            if (b.second->name.find("E0")!= std::string::npos){
                myfile<<"n "<<b.first->key<<" 1"<<endl;
            }
        }
    }
 }
 for(auto& it : graph){
    for(auto& a : it){
      for (auto& c: a->tgts)
      {
         if (c.second->name.find("E92")!= std::string::npos)
         {
              myfile<<"n "<<c.first->key<<" -1"<<endl;
         }
      }
    }
 }
 
        
 for(auto& it : graph)
 {
    for(auto& a : it)
    {
      for(auto& b : a->srcs)
      {
        myfile<<"a "<<b.first->key<<" "<<a->key<<" 0 1 "<<100*a->time<<endl;
      }
      for(auto& c : a->tgts){
        myfile<<"a "<<a->key<<" "<<c.first->key<<" "<<"0 1 "<<100*c.first->time<<endl;
      }
    }
 }
}
void writefile2(std::vector<std::vector<TimeExpandedNode*>> graph){
   for(auto& it : graph)
   {
    for(auto& a : it)
    {
      for(auto& b : a->srcs){
        myfile<<b.second->name<<" ";
      }
      myfile<<"||";
      myfile<<a->origin->x<<" "<<a->origin->y<<"||";
      if (a->tgts.empty())
      {
        myfile<<endl;
      }
      
      for(auto& c : a->tgts){
        myfile<<c.second->name<<" ";
      }
      myfile<<endl;
    }
   }
}

