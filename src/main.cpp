#include "rtree.hpp"
#include <iostream>

int main(){
    
    rtree first_node;
    rtree* root;

    first_node.set_the_first_node();
    first_node.rt_root()->rt_insert_edge(1,1,2,2);
    first_node.rt_root()->rt_insert_edge(2,2,3,3);
    first_node.rt_root()->rt_insert_edge(2,2,1,3);
    first_node.rt_root()->rt_insert_edge(3,3,4,4);
    first_node.rt_root()->rt_insert_edge(4,4,5,5);
    first_node.rt_root()->rt_insert_edge(5,5,6,6);
    first_node.rt_root()->rt_insert_edge(6,6,7,7);
    root = first_node.rt_root();
    root->rt_print(0);

    std::cout<<"Search in the tree. \r\n";
    std::vector<rtree*> result1, result2;
    root->rt_search(1.5,1.5,&result1);
    for(int i=0;i<result1.size();i++){
        std::cout<<"the node contains (1.5,1.5): "<<result1[i]->get_index()<<"\r\n";
    }
    std::cout<<"Search for k nearest neighbor. \r\n";
    root->rt_k_nearest_items(1.5,1.5,3,&result2);
    for(int i=result2.size()-1;i>=0;i--){
        std::cout<<"the "<<result2.size()-i<<"th nearest node to (1.5,1.5): "<<result2[i]->get_index()<<"\r\n";
    }
    root->rt_delete(3);
    root->rt_print(0);

    return 0;
}

