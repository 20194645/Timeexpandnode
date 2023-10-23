
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
     for(auto& vec : graph)
     {
      for(auto& it : vec){
        if(it->origin->equals(origin)&& it->time==time)
        {
            return it;
        }
      }
     }
     return nullptr;
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
    TimeExpandedNode* node = graph[m][n];
    std::queue<TimeExpandedNode*> Q;
    Q.push(node);
    
    while (!Q.empty()) {
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
bool compare(std::tuple<int, int, double>a ,std::tuple<int, int, double> b)
{
 return (get<2>(a) < get<2>(b));
}
std::vector<std::tuple<int, int, double>> getChains(std::vector<std::vector<TimeExpandedNode*>> graph, Point* origin)
{  
    std::vector<std::tuple<int, int, double>> a;
    int i,k;
    for(i = 0;i<graph.size();i++){
        for(k= 0;k<graph[i].size();k++)
         {
            if(graph.at(i).at(k)->origin->equals(origin))
            {
             a.push_back(make_tuple(i,k,graph[i][k]->time));
            }
         }
    }
    sort(a.begin(),a.end(),compare);
    return a;
}
std::vector<std::pair<int, double>> createNewChains(std::vector<std::tuple<int, int, double>> oldChains,
           std::vector<std::vector<TimeExpandedNode*>> graph, 
           double H, double deltaT)
{
    int count = 0,steps = 0;
    int oldSize = oldChains.size();
    int fixedindex = get<1>(oldChains.at(0));
    std::vector<std::pair<int, double>> newChains;
    while (count < oldSize -1)
    {
        auto& prev = oldChains.at(count);
        auto& next = oldChains.at(count+1);
        steps = 0;
        if(get<2>(prev)<get<2>(next)-deltaT)
        {
            steps = floor((get<2>(next)-get<2>(prev))/deltaT);
            int size = newChains.size( );
            for(int i = 0; i < steps; i++){
                newChains.push_back(make_pair(i+size,get<2>(prev)+deltaT*(i+1)));
            }
        }
        count++;
    }
    auto& last = oldChains.at(oldSize-1);
    steps =floor(H-get<2>(last));
    int size = newChains.size()+oldChains.size();
    int i ;
    for( i = 0; i < steps; i++){
		newChains.push_back(std::make_pair(i + size, get<2>(last) + deltaT*(1 + i)));
	}
	return newChains;

}
std::vector<std::pair<int, int>>insert2(std::vector<std::vector<TimeExpandedNode*>> &graph, std::vector<std::pair<int, double>> newChains, TimeExpandedNode* p)
{
    std::vector<std::pair<int, int>> result;
    for(auto& elem : newChains)
    {
        if(isAvailable(graph,p->origin,elem.second)!=nullptr)
        {

         continue;
        }
        TimeExpandedNode * newNode = new TimeExpandedNode();
        newNode->setTENode(p->origin);
        newNode->time = elem.second;
        newNode->tgts=p->tgts;
        if(graph.size() <= elem.first)
        {
            std::vector<TimeExpandedNode*> v;
            v.push_back(newNode);
            graph.push_back(v);
            result.push_back(make_pair(graph.size( ) - 1, 0));
        } 
        else{
		auto& currVector = graph.at(elem.first);
		if(currVector.size( ) == 0 || currVector.at(0)->time == elem.second ){
				graph.at(elem.first).push_back(newNode);
				result.push_back(make_pair(elem.first,graph.at(elem.first).size() -1));
		}
        else{
				std::vector<TimeExpandedNode*> v;
                v.push_back(newNode);
				graph.insert(graph.begin() + elem.first,1, v);
				result.push_back(std::make_pair(elem.first, 0));
		 }

        }

    }
    return result;
}
bool checkInsertion(std::vector<std::vector<TimeExpandedNode*>> graph, 
	std::vector<std::pair<int, double>> newChains, 
		TimeExpandedNode* p){
	for(auto& elem : newChains){
		if(isAvailable(graph, p->origin, elem.second)!=nullptr){
			return true;
		}
	}
	return false;
}
bool checkResult(std::vector<std::vector<TimeExpandedNode*>> graph, 
	std::vector<std::pair<int, double>> newChains, 
		std::vector<std::pair<int, int>> newPositions,
		TimeExpandedNode* p){
int i = 0;
	for(auto& elem : newChains){
        if(i<newPositions.size())
        {
         auto& pos = newPositions.at(i);
		 if(!graph.at(pos.first).at(pos.second)->origin->equals(p->origin) || 
			graph.at(pos.first).at(pos.second)->time != elem.second
			){
			return false;
		 }
        }
		i++;
	}
	return true;
}
std::vector<std::pair<int, int>>  merge(std::vector<std::tuple<int, int, double>> oldChains,
						std::vector<std::pair<int, int>> newChains){
	int count = 0, i = 0, j = 0;
	std::vector<std::pair<int, int>> result ;
	int sizeOld = oldChains.size( ), sizeNew = newChains.size( );
	int size = oldChains.size( ) + newChains.size( );
	while(count < size && i < sizeOld && j < sizeNew){
		if( get<0>(oldChains.at(i)) < newChains.at(j).first){
			result.push_back(std::make_pair(i, get<1>(oldChains.at(i))));
			i++;
		}
		else if(get<0>(oldChains.at(i)) > newChains.at(j).first){
			result.push_back(std::make_pair(j, newChains.at(j).second));
			j++;
		}
		else{ //trường hợp này nghĩa là get<0>(oldChains.at(i)) == newChains.at(j).first
			//tuy vậy điều này gần như không được - và không thể xảy ra
			result.push_back(std::make_pair(i, get<1>(oldChains.at(i))));
			i++; j++;
		}
		count++;
	}
	return result;
}

void assertNewOrder(std::vector<std::pair<int, int>> newOrder, std::vector<std::vector<TimeExpandedNode*>> graph)
{
    for(auto& e1 : newOrder){
	   for(auto& e2 : newOrder){
		if(e1.first < e2.first){			
           assert(graph.at(e1.first).at(e1.second)->time < graph.at(e2.first).at(e2.second)->time);
		}
	  }
    }
}


std::vector<std::vector<TimeExpandedNode*>> connectChains(
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

}
TimeExpandedNode* isAvailableminNode(std::vector<std::vector<TimeExpandedNode*>> graph, Point* origin)
{ 
    TimeExpandedNode* tmp = nullptr;
    double min = 9999;
    for(auto& vec : graph)
     {
      for(auto& it : vec){
        if(it->origin->equals(origin)&& it->time < min)
        {
            tmp = it;
            min = it->time;
        }
      }
     }
    return tmp;
}
void connectAllChains(std::vector<std::vector<TimeExpandedNode*>> &graph,std::vector<Point*> points,double H)
{
   for(auto& origin : points)
    {
        TimeExpandedNode* temp = isAvailableminNode(graph, origin); 
        std::vector<std::tuple<int, int, double>> chains = getChains(graph, origin);
        std::vector<std::pair<int, double>> newChains = createNewChains(chains,graph,H,1);
        if(!newChains.empty()){
         //cout<<newChains.size()<<endl;
         std::vector<std::pair<int, int>>newPos = insert2(graph, newChains, temp);
         assert(checkInsertion(graph, newChains, temp));
         //cout<<newChains.size()<<" "<<newPos.size()<<endl; 
	     assert(checkResult(graph, newChains, newPos, temp));
         for(auto& elem : newPos){
            spread(graph, elem.first, elem.second, 1000);
	     }
         std::vector<std::pair<int, int>> newOrder = merge(chains, newPos);
         connectChains(graph, newOrder);
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
    }
  }
  for ( i = 0; i < sumten; i++){
    if(x[i]==0){
        return false;
        //count++;
    } 
  }
  return true;
}
void writeFile(std::vector<std::vector<TimeExpandedNode*>> graph)
{
 for(auto& it : graph)
 {
    for(auto& a : it)
    {
        if(!a->srcs.empty())
        {
         for(auto& b : a->srcs)
         {
         if(!a->tgts.empty())
          { for(auto& c : a->tgts)
           {
            myfile<<a->key<<" "<<b.first->key<<" "<<c.first->key<<" "<<"0 1"<<endl;
           }
          }
          else{
             myfile<<a->key<<" "<<b.first->key<<" "<<"NULL"<<" "<<"0 1"<<endl;
          }
         }
        }
        else{
            for(auto& c : a->tgts)
            {
                 myfile<<a->key<<" "<<"NULL"<<" "<<c.first->key<<" "<<"0 1"<<endl;
            }
        }
    }
 }
}




