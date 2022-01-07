#ifndef RTREE_H
#define RTREE_H

#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <math.h> 

#define CHILD_SIZE 4
#define MAX_DOUBLE 1e300
#define MIN_DOUBLE -1e300

/* Each rtree object is either a node or an item.
 * A node is literally node of a rtree.
 * An item is the thing we save in a tree, such as
 * an edge of a graph.
 * Note that they are all node in codes, but they are
 * still from different concept.
 * A root node's parent is itself.
 * A leaf node's every children are items.
 */
class rtree;

class rtree{
public:    
    /* vrt: vector of rtree node: store each node 
     * of the tree in a vector.
     */ 
    static std::vector<rtree*> vrt;
    /* The Bounding Box (BB) of currrent node
     */
    double pxmin=MAX_DOUBLE, pymin=MAX_DOUBLE, pxmax=MIN_DOUBLE, pymax=MIN_DOUBLE;
    /* The start point (x1, y1) and end point (x2, y2) of 
     * currrent item, which is an edge.
     */
    double px1=0, py1=0, px2=0, py2=0;
    /* Constructor
     */
    rtree();
    rtree(double xmin, double ymin, double xmax, double ymax);
    ~rtree();
    /* Gives node's index in vrt
     */
    unsigned long get_index();
    /* Gives the size of the area
     */
    double get_area();
    /* return 1 if it is leaf node
     */
    int is_leaf();
    /* set yes = 1 to make the node a leaf
     * otherwise, set 0
     */
    int set_leaf(int yes);
    /* return 1 if it is an edge
     */
    int is_edge();
    /* set yes = 1 to make the node an edge
     */
    int set_edge(int yes);
    /* If this is the first node of a tree, call this function.
     * This node can not be the item to stored.
     * This function set the node to be both leaf and root.
     */
    int set_the_first_node();
    /* Set the node's parent
     */
    int set_parent(rtree* pr);
    /* Check if given bounding box is inside of current node.
     */
    int check_include(double xmin, double ymin, double xmax, double ymax);
    /* Check the area increment when the current node's 
     * bounding box is extended to include given bounding box
     */
    double get_increment(double xmin, double ymin, double xmax, double ymax);
    /* Change the current node's bounding box to fit the given
     * bounding box
     */
    int set_increment(double xmin, double ymin, double xmax, double ymax);
    /* Add a node or an item to current node's child
     */
    int rt_addchild(rtree* child_ptr);
    /* Insert an item to current node's subtree.
     * It also returns the item's pointer.
     * An example for inserrtion: 
     * rtree theNode;
     * theNode.set_the_first_node();
     * theNode.rt_root()->rt_insert(xmin,ymin,xmax,ymax);
     */
    rtree* rt_insert(double xmin, double ymin, double xmax, double ymax);
    /* Insert an edge of a graph.
     * (x1, y1) doesn't have to be (xmin, ymin), and
     * (x2, y2) doesn't have to be (xmax, ymax).
     */
    rtree* rt_insert_edge(double x1, double y1, double x2, double y2);
    /* Return the root node of this rtree.
     */
    rtree* rt_root();
    /* Print the node's subtree
     */
    int rt_print(int depth);
    /* Remove an item.
     * However, this function won't delete any node.
     * TODO: If underflow, delete the parent at the same time,
     * then insert the orphans one by one
     */
    int rt_delete(int rm_index);
    /* Fill the result vector with the pointer of leaf nodes that includes (x, y).
     * If a node's children doesn't contain (x, y), the node's index
     * will be filled, too.
     */
    int rt_search(double x,double y, std::vector<rtree*>* result);
    /* This method is wrapped by rt_k_nearest_items().
     * item_result queue stores the items by order, where the farest item
     * is at the top of the queue.
     */
    int knn_iter(double x, double y, int k, std::priority_queue<std::pair<double,rtree*>>* item_result);
    /* Fill 1st to kth nearest items in the result vector,
     * but in reverse order. That is, result[0] stores the
     * farest node in those knn nodes.
     * Also, it returns the distance between the nearest item and (x, y)
     * NOTE: If there are multiple items with same distance to (x,y), some may be discarded in the final result.
     */
    double rt_k_nearest_items(double x, double y, int k, std::vector<rtree*>* result);
private:
    /* Store the node's index in vrt.
     */
    unsigned long index;
    /* If the node is a leaf, leaf = 1. 
     */
    int leaf=0;
    /* A vector that stores all the child's pointer of a node.
     */
    std::vector<rtree*> child;
    /* Parent's pointer.
     */
    rtree* parent;
    /* If this is an item, and the item is an edge in the 2D space,
     * edge = 1.
     * Also it will have px1, py1, px2, py2 value.
     */
    int edge = 0;
};
/* comment TODO:
 * Who will call this function? (root only?)
 * Will it change any properties? (add_child may change child's parent either)
 * Return value meaning
 */

#endif