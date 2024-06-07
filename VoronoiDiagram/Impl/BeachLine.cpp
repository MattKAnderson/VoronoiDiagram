#include <VoronoiDiagram/Impl/BeachLine.hpp>

namespace VoronoiDiagram::Impl {

Arc* BeachLine::find_intersected_arc(const RealCoordinate& c) {
    Arc* node = head;
    double y;
    //std::cout << "c: (" << c.x << ", " << c.y << ")" << std::endl;
    // TODO: handle case when c.y == y
    //       it is super rare but I've seen it at 100k seeds
    while (true) {
        if (node->left != nullptr) {
            if (node->focus.x < node->upper->focus.x && c.y > node->upper->focus.y) {
                node = node->left;
                continue;
            }
            if (node->focus.x <= node->upper->focus.x || c.y > node->focus.y) {
                y = parabolae_y_intercept(c.x, node->upper->focus, node->focus);
                if (c.y > y) {
                    node = node->left;
                    continue;
                } 
            }
        }
        if (node->right != nullptr) {
            if (node->focus.x < node->lower->focus.x && c.y < node->lower->focus.y) {
                node = node->right;
                continue;
            }
            if (node->focus.x <= node->lower->focus.x || c.y < node->focus.y) {
                y = parabolae_y_intercept(c.x, node->focus, node->lower->focus);
                if (c.y < y) {
                    node = node->right;
                    continue;
                }
            }
        }
        return node;
    }
}   

Arc* BeachLine::get_lowest() {
    Arc* lowest = head;
    while (lowest->right != nullptr) { lowest = lowest->right; }
    return lowest;
}

void BeachLine::insert_arc_above(Arc* arc, Arc* new_arc) {
    if (arc->left == nullptr) {
        arc->left = new_arc;
        new_arc->parent = arc;
        if (arc->upper != nullptr) {
            arc->upper->lower = new_arc;
        }
    }
    else {
        arc->upper->right = new_arc;
        new_arc->parent = arc->upper;
        arc->upper->lower = new_arc;
    }
    new_arc->upper = arc->upper;
    new_arc->lower = arc;
    arc->upper = new_arc;
    //insert_balance(new_arc);
}

void BeachLine::insert_balance(Arc* arc) {
    arc->red = true;
    arc = arc->parent;
    Arc* parent = arc->parent;
    while (parent != nullptr) {
        if (parent->left == arc) { parent->left = insert_local_balance(arc); }
        else { parent->right = insert_local_balance(arc); }
        arc = parent;
        parent = parent->parent;
    }
    arc = insert_local_balance(arc);
    arc->red = false;
    head = arc;
}

Arc* BeachLine::insert_local_balance(Arc* arc) {
    if (is_red(arc->right) && !is_red(arc->right)) { arc = rotate_left(arc); }
    if (is_red(arc->left) && is_red(arc->left->left)) { arc = rotate_right(arc); }
    if (is_red(arc->left) && is_red(arc->right)) { flip_colors(arc); }
    return arc;
}

void BeachLine::delete_balance(Arc* arc) {
    // TODO
}

Arc* BeachLine::rotate_left(Arc* arc) {
    Arc* rchild = arc->right;
    rchild->parent = arc->parent;
    arc->parent = rchild;
    arc->right = rchild->left;
    if (arc->right) { arc->right->parent = arc; }
    arc->right->parent = arc;
    rchild->left = arc;
    rchild->red = arc->red;
    arc->red = true;
    return rchild;
}

Arc* BeachLine::rotate_right(Arc* arc) {
    Arc* lchild = arc->left;
    lchild->parent = arc->parent;
    arc->parent = lchild;
    arc->left = lchild->right;
    if (arc->left) { arc->left->parent = arc; }
    lchild->right = arc;
    lchild->red = arc->red;
    arc->red = true;
    return lchild;
}

void BeachLine::flip_colors(Arc* arc) {
    arc->red = true;
    arc->left->red = false;
    arc->right->red = false;
}

bool BeachLine::is_red(Arc* arc) {
    return arc != nullptr && arc->red;
}

void BeachLine::remove_arc(Arc* arc) {
    // Handle the doubly linked list portion of this
    // Data structure
    if (arc->upper){
        arc->upper->lower = arc->lower;
    }
    if (arc->lower) {
        arc->lower->upper = arc->upper;
    }

    if (arc->left == nullptr) {
        if (arc->right == nullptr) {
            // leaf node case
            if (arc == head) { head = nullptr; }
            else if (arc->parent->left == arc) { arc->parent->left = nullptr; }
            else { arc->parent->right = nullptr; }

        }
        else {
            // single child on right case
            arc->right->parent = arc->parent;
            if (arc == head) { head = arc->right; }
            else if (arc->parent->left == arc) { arc->parent->left = arc->right; }
            else {arc->parent->right = arc->right; }
        } 
    }
    else if (arc->right == nullptr) {
        // single child on left case
        arc->left->parent = arc->parent;
        if (arc == head) { head = arc->left; }
        else if (arc->parent->left == arc) { arc->parent->left = arc->left; }
        else {arc->parent->right = arc->left; }
    }
    else {
        // two children case
        Arc* upper = arc->upper;

        // connect upper's child to upper's parent
        if (upper->parent->left == upper) { upper->parent->left = upper->left; }
        else { upper->parent->right = upper->left; }
        if (upper->left) { upper->left->parent = upper->parent; }

        // put upper in place of arc
        upper->parent = arc->parent;
        upper->left = arc->left;
        upper->right = arc->right;
        upper->red = arc->red;
        if (arc->left) { arc->left->parent = upper; }
        if (arc->right) { arc->right->parent = upper; }
        if (arc == head) { head = upper; }
        else if (arc->parent->left == arc) { arc->parent->left = upper; }
        else { arc->parent->right = upper; }
    }
    //available_arcs.push_back(arc);
    // TODO: delete_balance(y);
}
} // namespace VoronoiDiagram::Impl