#include "rtree.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

    first_node.set_the_first_node();
    for(int i=0; i<vstr.size(); i++){
        double x1,y1,x2,y2;
        sscanf(vstr.at(i).c_str(),"x1: %lf, y1: %lf, x2: %lf, y2: %lf, map: jj_demo",&x1,&y1,&x2,&y2);
        first_node.rt_root()->rt_insert_edge(x1,y1,x2,y2);

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
    root->rt_print(0);
    
    //std::cout<<"Search in the tree. \r\n";
    std::vector<rtree*> result1, result2;
    double px = 4,py = 2.5;
    //root->rt_search(px,py,&result1);
    //for(int i=0;i<result1.size();i++){
    //    std::cout<<"the node contains ("<<px<<","<<py<<"): "<<result1[i]->get_index()<<"\r\n";
    //}
    std::cout<<"Search for k nearest neighbor. \r\n";
    double dis = root->rt_k_nearest_items(px,py,3,&result2);
    std::cout<<"dis = "<<dis<<std::endl;
    for(int i=result2.size()-1;i>=0;i--){
        std::cout<<"the "<<result2.size()-i<<"th nearest node to ("<<px<<","<<py<<"): "<<result2[i]->get_index()<<"\r\n";
    }
    /*
    root->rt_delete(3);
    root->rt_print(0);
    */
    return 0;
}

