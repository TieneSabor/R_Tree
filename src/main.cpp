#include "rtree.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>

double violent_knn(double* a, int n, int k, double x, double y, std::priority_queue<std::pair<double,int>>* item_result){
    auto before = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    for(int i=0;i<n;i++){
        double x1=a[4*i];
        double y1=a[4*i+1];
        double x2=a[4*i+2];
        double y2=a[4*i+3];
        double norm = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
        double dist;
        
        if(norm>1e-6)
        {
            double cos = ( x2- x1)/norm;
            double sin = ( y2- y1)/norm;
            //double sin = sqrt(1-pow(cos,2));
            double xrot =  cos*(x-(x1+x2)/2) + sin*(y-( y1+ y2)/2);
            double yrot = -sin*(x-(x1+x2)/2) + cos*(y-( y1+ y2)/2);
            double xtrim = std::fmax(abs(xrot)-norm/2,0);
            double ytrim = std::fmax(abs(yrot),0);
            dist = sqrt(xtrim*xtrim+ytrim*ytrim);
        }
        else
        {
            dist = sqrt(pow(x- x1,2)+pow(y- y1,2));
        }
/*
	double cos;
	if(norm==0){
	    cos=0;
	}
	else{
	    cos = (x2-x1)/norm;
	}
	double sin = sqrt(1-pow(cos,2));
	//double sin = (y2-y1)/norm;
	double xrot =  cos*(x-(x1+x2)/2) + sin*(y-(y1+y2)/2);
	double yrot = -sin*(x-(x1+x2)/2) + cos*(y-(y1+y2)/2);
	double xtrim = std::fmax(abs(xrot)-norm/2,0);
	double ytrim = std::fmax(abs(yrot),0);
	double dist = sqrt(xtrim*xtrim+ytrim*ytrim);
*/
        item_result->push(std::make_pair(dist, i));
        while(item_result->size()>k){
            item_result->pop();
        }
    }
    auto after = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::pair<double,int> min = item_result->top();
    unsigned long ms = after-before;
    std::cout<<"time spent ms: "<<ms<<", the closest dist by violence: "<<min.first<<", (x1,y1,x2,y2): ("<<a[4*min.second]<<","<<a[4*min.second+1]<<","<<a[4*min.second+2]<<","<<a[4*min.second+3]<<")"<<std::endl;
    return min.first;
}

int main(){
   
    std::ifstream file("./data/nodes01.txt");
    std::string buf;
    std::vector<std::string> vstr;
    while(std::getline(file, buf)){
        //std::cout<<buf<<std::endl;
        vstr.push_back(buf);
    }

    rtree first_node;
    rtree* root;
    int num=0;
    first_node.set_the_first_node();
    int offset=0;
    /*
    for(int i=0; i<vstr.size(); i++){
        double x1,y1,x2,y2;
        sscanf(vstr.at(i).c_str(),"x1: %lf, y1: %lf, x2: %lf, y2: %lf, map: jj_demo",&x1,&y1,&x2,&y2);
        first_node.rt_root()->rt_insert_edge(x1,y1,x2,y2);
	a[4*i]   = x1;
	a[4*i+1] = y1;
	a[4*i+2] = x2;
	a[4*i+3] = y2;
	num++;
    }
    offset+=vstr.size();
    *//*
    int exptime = 200;
    double* a = new double[4*70*exptime];
    for(int i=0; i<vstr.size()*exptime; i++){
        double x1,y1,x2,y2;
        sscanf(vstr.at(i%vstr.size()).c_str(),"x1: %lf, y1: %lf, x2: %lf, y2: %lf, map: jj_demo",&x1,&y1,&x2,&y2);
        first_node.rt_root()->rt_insert_edge(x1,y1,x2,y2);
	a[4*(i)]   = x1;
	a[4*(i)+1] = y1;
	a[4*(i)+2] = x2;
	a[4*(i)+3] = y2;
	num++;
    }
    */

    srand(time(NULL));
    int testsize = 10000;
    double* a = new double[4*testsize];
    double max=100,min=-100;
    for(int i=0; i<testsize; i++){
        double x1,y1,x2,y2;
	    x1 = (max - min) * rand() / (RAND_MAX + 1.0) + min;
	    y1 = (max - min) * rand() / (RAND_MAX + 1.0) + min;
	    x2 = (max - min) * rand() / (RAND_MAX + 1.0) + min;
	    y2 = (max - min) * rand() / (RAND_MAX + 1.0) + min;
        first_node.rt_root()->rt_insert_edge(x1,y1,x1,y1);
	    a[4*(i)]   = x1;
	    a[4*(i)+1] = y1;
	    a[4*(i)+2] = x1;
	    a[4*(i)+3] = y1;
	    num++;
    }

    /*
    first_node.rt_root()->rt_insert_edge(1,1,2,2);
    first_node.rt_root()->rt_insert_edge(2,2,3,3);
    first_node.rt_root()->rt_insert_edge(2,2,1,3);
    first_node.rt_root()->rt_insert_edge(3,3,4,4);
    first_node.rt_root()->rt_insert_edge(4,4,5,5);
    first_node.rt_root()->rt_insert_edge(5,5,6,6);
    first_node.rt_root()->rt_insert_edge(6,6,7,7);
    */
    root = first_node.rt_root();
    //root->rt_print(0);
    
    //std::cout<<"Search in the tree. \r\n";
    std::vector<rtree*> result1, result2;
    std::priority_queue<std::pair<double,int>> q;
    double px = 0,py = 0;
    //root->rt_search(px,py,&result1);
    //for(int i=0;i<result1.size();i++){
    //    std::cout<<"the node contains ("<<px<<","<<py<<"): "<<result1[i]->get_index()<<"\r\n";
    //}
    violent_knn(a,testsize,1,px,py,&q);
    std::cout<<"Search for k nearest neighbor. \r\n";
    auto before = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double dis = root->rt_k_nearest_items(px,py,1,&result2);
    auto after = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout<<"dis = "<<dis<<std::endl;
    unsigned long ms = after-before;
    std::cout<<"rtree ms: "<<ms<<", bf: "<<before<<", af:"<<after<<std::endl;
    for(int i=result2.size()-1;i>=0;i--){
        std::cout<<"the "<<result2.size()-i<<"th nearest node to ("<<px<<","<<py<<") (x1,y1,x2,y2): ("<<result2[i]->pxmin<<","<<result2[i]->pymin<<","<<result2[i]->pxmax<<","<<result2[i]->pymax<<")"<<std::endl;
    
    }
    /*
    root->rt_delete(3);
    root->rt_print(0);
    */
    return 0;
}

