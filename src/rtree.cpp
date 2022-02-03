#include "rtree.hpp"

std::vector<rtree*> rtree::vrt;

int stack_cnt=0;

double max(double a, double b){
    if(a>=b){
        return a;
    }
    else{
        return b;
    }
}

double min(double a, double b){
    if(a<=b){
        return a;
    }
    else{
        return b;
    }
}

rtree::rtree(){
    index = vrt.size();
    vrt.push_back(this);
}

rtree::rtree(double xmin, double ymin, double xmax, double ymax){
    index = vrt.size();
    vrt.push_back(this);
    pxmin = xmin;
    pymin = ymin;
    pxmax = xmax;
    pymax = ymax;
}

rtree::~rtree(){
    // Remove the item from the vrt?
    vrt.erase(vrt.begin()+index);
}

unsigned long rtree::get_index(){
    return index;
}

double rtree::get_area(){
    return (pxmax-pxmin)*(pymax-pymin);
}

int rtree::is_leaf(){
    return leaf;
}

int rtree::set_leaf(int yes){
    if(yes){
        leaf = 1;
    }
    else{
        leaf = 0;
    }
    return 1;
}

int rtree::is_edge(){
    return edge;
}

int rtree::set_edge(int yes){
    edge = yes;
    return 1;
}

int rtree::set_the_first_node(){
    parent = this;
    leaf = 1;
    return 1;
}

int rtree::set_parent(rtree* pr){
    parent = pr;
    return 1;
}

int rtree::check_include(double xmin, double ymin, double xmax, double ymax){
    return (xmin>pxmin)&&(ymin>pymin)&&(xmax<pxmax)&&(ymax<pymax);
}

double rtree::get_increment(double xmin, double ymin, double xmax, double ymax){
    if(check_include(xmin, ymin, xmax, ymax)){
        //std::cout<<"the object going to be inserted is already included in this node:"<<index<<" \r\n";
        return 0;
    }
    else{
        //return (((max(xmax,pxmax)-min(xmin,pxmin))*(max(ymax,pymax)-min(ymin,pymin)))-((pxmax-pxmin)*(pymax-pymin)))/((pxmax-pxmin)*(pymax-pymin)); 
        return (((max(xmax,pxmax)-min(xmin,pxmin))*(max(ymax,pymax)-min(ymin,pymin))));
    }
}

int rtree::set_increment(double xmin, double ymin, double xmax, double ymax){
    pxmin=min(pxmin,xmin);
    pymin=min(pymin,ymin);
    pxmax=max(pxmax,xmax);
    pymax=max(pymax,ymax);
    return 1;
}

int rtree::rt_addchild(rtree* child_ptr){
    /* Add the new child to the vector anyway.
     */
    //stack_cnt++;
    child.push_back(child_ptr);
    child_ptr->set_parent(this);
    set_increment(child_ptr->pxmin,child_ptr->pymin,child_ptr->pxmax,child_ptr->pymax);
    /* If the node have enough space for child
     * do nothing here.
     */
    if(child.size()<=CHILD_SIZE){
        return 1;
    }
    /* If not, we will have to split the child
     * to 2 groups, belongs to 2 nodes, and 
     * make both nodes children of the parent.
     * Practically, we modify the old node and
     * add another new one to the parent.
     * BTW, we use the quadratic split to choose
     * partition seeds.
     */
    else{
        /* Choose the seeds with max increased area
         */
        int seedi=0, seedj=0;
        double maxIncreasedArea=0;
        for(int i=0;i<child.size();i++){
            for(int j=i+1;j<child.size();j++){
                //TODO
                double IncreasedArea = 
                    child[i]->get_increment(child[j]->pxmin,child[j]->pymin,child[j]->pxmax,child[j]->pymax);
                //IncreasedArea -= (child[j]->get_area());// the area of i had been substracted in "get_increment()"
                if(IncreasedArea >= maxIncreasedArea){
                    maxIncreasedArea = IncreasedArea;
                    seedi = i;
                    seedj = j;
                }
            }
        }
        /* Put current child, except seeds, into another vector
         * and seperate them later
         */
        std::vector<rtree*> ToBeSeperated;
        rtree* seedi_ptr = child[seedi];
        rtree* seedj_ptr = child[seedj];
        int i=child.size();
        while(child.size()){
            i--;
            if((i!=seedi)&&(i!=seedj)){
                ToBeSeperated.push_back(child.at(i));
            }
            child.pop_back();
        }
        /* Create another node.
         * Add rest children to this and another
         * node one by one.
         */
        // Initialize both nodes
        pxmin = MAX_DOUBLE;
        pymin = MAX_DOUBLE;
        pxmax = MIN_DOUBLE;
        pymax = MIN_DOUBLE;
        rtree* new_node = new rtree;
        /* The new node will be a leaf if and only if
         * its sibling that 'splitted' this new node 
         * is also a leaf.
         */
        if(is_leaf()){
            new_node->set_leaf(true);
        }
        // Put seeds into both nodes
        //new_node->set_increment(seedi_ptr->pxmin,seedi_ptr->pymin,seedi_ptr->pxmax,seedi_ptr->pymax);
        new_node->rt_addchild(seedi_ptr);
        child.push_back(seedj_ptr);
        set_increment(seedj_ptr->pxmin,seedj_ptr->pymin,seedj_ptr->pxmax,seedj_ptr->pymax);
        // Put the child in node with smallest increment.
        i = ToBeSeperated.size();
        while(ToBeSeperated.size()){
            i--;
            double IncrementForSeedi = 
                seedi_ptr->get_increment(ToBeSeperated[i]->pxmin,ToBeSeperated[i]->pymin,ToBeSeperated[i]->pxmax,ToBeSeperated[i]->pymax);
            double IncrementForSeedj = 
                seedj_ptr->get_increment(ToBeSeperated[i]->pxmin,ToBeSeperated[i]->pymin,ToBeSeperated[i]->pxmax,ToBeSeperated[i]->pymax);
            if((IncrementForSeedi<=IncrementForSeedj)&&(new_node->get_child_size()<=(CHILD_SIZE-2)/2)){
                new_node->rt_addchild(ToBeSeperated[i]);
                //new_node->set_increment(ToBeSeperated[i]->pxmin,ToBeSeperated[i]->pymin,ToBeSeperated[i]->pxmax,ToBeSeperated[i]->pymax);
            }
            else{
                child.push_back(ToBeSeperated[i]);
                set_increment(ToBeSeperated[i]->pxmin,ToBeSeperated[i]->pymin,ToBeSeperated[i]->pxmax,ToBeSeperated[i]->pymax);
            }
            ToBeSeperated.pop_back();
        }
        /* Add the new_node to the parent.
         */
        if(parent==this){// root node
            rtree* root = new rtree;
            root->set_parent(root);
            root->rt_addchild(this);
            root->rt_addchild(new_node);
        }
        else{// not root node
            parent->rt_addchild(new_node);
        }
        return 1;
    }
}

// assume the object to be inserted is smaller than the current BB
rtree* rtree::rt_insert(double xmin, double ymin, double xmax, double ymax){
    if(!check_include(xmin,ymin,xmax,ymax)){
        set_increment(xmin,ymin,xmax,ymax);
    }
    if(!leaf){// not a leaf node.
        /* Check for each child to see if 
         * the object going to be inserted
         * can be inserted into child.
         */
        for(int i=0;i<child.size();i++){
            if(child[i]->check_include(xmin, ymin, xmax, ymax)){
                return child[i]->rt_insert(xmin, ymin, xmax, ymax);
            }
        }
        /* Come here only when there's no
         * child that can handle the new object.
         * We will need to find a child that
         * can handle the new object with minimal
         * expansion and then insert.
         */
        int min_index=-1;
        double min_expansion=MAX_DOUBLE;
        for(int i=0;i<child.size();i++){
            double expansion = child[i]->get_increment(xmin, ymin, xmax, ymax);
            if(min_expansion>expansion){
                min_expansion=expansion;
                min_index=i;
            }
        }
        child[min_index]->set_increment(xmin, ymin, xmax, ymax);
        return child[min_index]->rt_insert(xmin, ymin, xmax, ymax);
    }
    else{// a leaf node
        rtree* new_child = new rtree(xmin, ymin, xmax, ymax);
        int success = rt_addchild(new_child);
        return new_child;
    }
}

rtree* rtree::rt_insert_edge(double x1, double y1, double x2, double y2){
    double xmin = min(x1, x2);
    double ymin = min(y1, y2);
    double xmax = max(x1, x2);
    double ymax = max(y1, y2);
    rtree* ToBeInserted = rt_insert(xmin, ymin, xmax, ymax);
    ToBeInserted->set_edge(1);
    ToBeInserted->px1 = x1;
    ToBeInserted->py1 = y1;
    ToBeInserted->px2 = x2;
    ToBeInserted->py2 = y2;
    return ToBeInserted;
}

rtree* rtree::rt_root(){
    if(parent!=this){//If this is not the root, return the parent's root
        return parent->rt_root();
    }
    else{//This is the root.
        return this;
    }
}

int rtree::rt_print(int depth){
    for(int i=0;i<child.size();i++){
        child[i]->rt_print(depth+1);
    }
    std::cout<<"[RTREE] This is node "<<index<<" at depth: "<<depth<<", the bounding box is ("<<pxmin<<","<<pymin<<"), ("<<pxmax<<","<<pymax<<"), leaf: "<<leaf<<", edge: "<<edge<<"\r\n";
    return 1;
}

int rtree::rt_delete(int rm_index){
    rtree* rm_parent = vrt[rm_index]->parent;
    for(int i=0;i<rm_parent->child.size();i++){
        if(rm_parent->child[i]->get_index()==rm_index){
            rm_parent->child.erase(rm_parent->child.begin()+i);
        }
    }
    delete vrt[rm_index];
    return 1;
}

int rtree::rt_search(double x,double y, std::vector<rtree*>* result){
    if(check_include(x,y,x,y)){
        if(leaf){
            result->push_back(this);
            return 1;
        }
        else{
            int success = -1;
            for(int i=0;i<child.size();i++){
                success = child[i]->rt_search(x,y,result);
            }
            /* Even when the node doesm't have any child,
             * it's index will be filled if it's bounding
             * box includes (x,y).
             */
            if((success==0)||(success==-1)){
                result->push_back(this);
            }
            return 1;
        }
    }
    else{
        return 0;
    }
}

int rtree::knn_iter(double x, double y, int k, std::priority_queue<std::pair<double,rtree*>>* item_result){
    /* We first sort all children by the distance between
     * (x, y) and the node bounding box (or the edge)
     */
    std::priority_queue<std::pair<double,rtree*>> child_queue;
    for(int i=0;i<child.size();i++){
        double dist=0;
        if(child[i]->is_edge()){// children are items and also edge.
            double norm = sqrt((child[i]->px2-child[i]->px1)*(child[i]->px2-child[i]->px1)+(child[i]->py2-child[i]->py1)*(child[i]->py2-child[i]->py1));
            // In case the edge is actually close to a 'point'.
            if(norm>1e-6)
            {
                double cos = (child[i]->px2-child[i]->px1)/norm;
                double sin = (child[i]->py2-child[i]->py1)/norm;
                double xrot =  cos*(x-(child[i]->px1+child[i]->px2)/2) + sin*(y-(child[i]->py1+child[i]->py2)/2);
                double yrot = -sin*(x-(child[i]->px1+child[i]->px2)/2) + cos*(y-(child[i]->py1+child[i]->py2)/2);
                double xtrim = max(abs(xrot)-norm/2,0);
                double ytrim = max(abs(yrot),0);
                dist = sqrt(xtrim*xtrim+ytrim*ytrim);
            }
            else
            {
                dist = sqrt(pow(x-child[i]->px1,2)+pow(y-child[i]->py1,2));
            }
            item_result->push(std::make_pair(dist,child[i]));
            //std::cout<<"node "<<index<<"'s child node "<<child[i]->index<<" is an edge with dist "<<dist<<"\r\n";
            //std::cout<<"child node "<<child[i]->index<<" norm:"<<norm<<", cos:"<<cos<<", sin:"<<sin<<", xrot:"<<xrot<<", yrot:"<<yrot<<", xtrim:"<<xtrim<<"\r\n";
        }
        else if(is_leaf()){// children are still items, but they are not edge.
            double width = child[i]->pxmax-child[i]->pxmin;
            double length = child[i]->pymax-child[i]->pymin;
            double centerx = (child[i]->pxmax+child[i]->pxmin)/2;
            double centery = (child[i]->pymax+child[i]->pymin)/2;
            double xtrim = max(abs(x)-centerx-width/2,0);
            double ytrim = max(abs(y)-centery-length/2,0);
            dist = sqrt(xtrim*xtrim+ytrim*ytrim);
            item_result->push(std::make_pair(dist,child[i]));
            //std::cout<<"node "<<index<<"'s child node "<<child[i]->index<<" is an item with dist "<<dist<<"\r\n";
            //std::cout<<"child node "<<child[i]->index<<" width:"<<width<<", length:"<<length<<", xtrim:"<<xtrim<<", ytrim:"<<ytrim<<"\r\n";
        }
        else{// children are nodes
            double width = child[i]->pxmax-child[i]->pxmin;
            double length = child[i]->pymax-child[i]->pymin;
            double centerx = (child[i]->pxmax+child[i]->pxmin)/2;
            double centery = (child[i]->pymax+child[i]->pymin)/2;
            double xtrim = max(abs(x)-centerx-width/2,0);
            double ytrim = max(abs(y)-centery-length/2,0);
            dist = sqrt(xtrim*xtrim+ytrim*ytrim);
            child_queue.push(std::make_pair(-dist,child[i]));
            //std::cout<<"node "<<index<<"'s child node "<<child[i]->index<<" is a node with dist "<<dist<<"\r\n";
            //std::cout<<"child node "<<child[i]->index<<" width:"<<width<<", length:"<<length<<", xtrim:"<<xtrim<<", ytrim:"<<ytrim<<"\r\n";
        }
    }
    /* Pop the farest item. 
     * We want only K nearest items
     */
    while(item_result->size()>k){
        item_result->pop();
    }
    //std::cout<<"node "<<index<<" have child_queue size: "<<child_queue.size()<<"\r\n";
    /* Then we start from nearest children.
     * We put children's offspring item into
     * priority queue with the distance as keys.
     * The action stop when the cloest distance
     * of un-searched node is larger than
     * the longest distance in current top k closest items. 
     * (and when the size of the queue is larger than k)
     */
    int result;
    while(child_queue.size()>0){
        std::pair<double,rtree*> cur_child = child_queue.top();
        double nearest_bb_dist = -cur_child.first;
        double farest_item_dist = MAX_DOUBLE;
        // Prevent getting random number when item_result queue is empty.
        if(item_result->size()>0){
            farest_item_dist = item_result->top().first;
        }
        if((nearest_bb_dist>farest_item_dist)&&(item_result->size()>=k)){
            // KNN have been found and stop the recursion.
            break;
        }
        else{
            // It is still possible to find closer item.
            result &= cur_child.second->knn_iter(x,y,k,item_result);
        }
        child_queue.pop();
    }
    return result;
}

double rtree::rt_k_nearest_items(double x, double y, int k, std::vector<rtree*>* result){
    std::priority_queue<std::pair<double,rtree*>> item_result;
    int res = knn_iter(x,y,k,&item_result);
    double dis = MAX_DOUBLE;
    while(item_result.size()>0){
        result->push_back(item_result.top().second);
        if(item_result.size()==1){
            dis = item_result.top().first;
        }
        item_result.pop();
    }
    return dis;
}
