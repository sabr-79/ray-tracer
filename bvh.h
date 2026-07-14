#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>
#include <vector>

class bvh_node : public hittable {
public:
    // Public constructors (unchanged)
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        // Copy the vector – safe, and we sort the copy during build
        this->objects = objects;
        root = build(start, end);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Iterative stack – no recursion, no shared_ptr overhead
        std::vector<int> stack;
        stack.reserve(64);          // typical depth is ~log2(n)
        stack.push_back(root);

        bool hit_anything = false;
        double closest = ray_t.max;

        while (!stack.empty()) {
            int idx = stack.back();
            stack.pop_back();
            const Node& node = nodes[idx];

            if (!node.bbox.hit(r, ray_t))
                continue;

            if (node.object_index >= 0) {
                // Leaf: intersect with actual object
                if (objects[node.object_index]->hit(r, interval(ray_t.min, closest), rec)) {
                    hit_anything = true;
                    closest = rec.t;
                }
            } else {
                // Internal: push children
                stack.push_back(node.left);
                stack.push_back(node.right);
            }
        }
        return hit_anything;
    }

    aabb bounding_box() const override { return nodes[root].bbox; }

private:
    struct Node {
        aabb bbox;
        int left;          // child indices, -1 for leaf
        int right;
        int object_index;  // index into `objects` for leaves, -1 for internal
    };

    std::vector<shared_ptr<hittable>> objects;   // own copy – safe
    std::vector<Node> nodes;
    int root;

    // Build recursively, returning node index
    int build(size_t start, size_t end) {
        Node node;
        node.bbox = aabb::empty;
        for (size_t i = start; i < end; ++i)
            node.bbox = aabb(node.bbox, objects[i]->bounding_box());

        int axis = node.bbox.longest_axis();
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t span = end - start;
        if (span == 1) {
            node.left = node.right = -1;
            node.object_index = static_cast<int>(start);
            nodes.push_back(node);
            return static_cast<int>(nodes.size()) - 1;
        } else if (span == 2) {
            node.left = build(start, start + 1);
            node.right = build(start + 1, end);
            node.object_index = -1;
            nodes.push_back(node);
            return static_cast<int>(nodes.size()) - 1;
        } else {
            // Sort only the range we're building
            std::sort(objects.begin() + start, objects.begin() + end, comparator);
            size_t mid = start + span / 2;
            node.left = build(start, mid);
            node.right = build(mid, end);
            node.object_index = -1;
            nodes.push_back(node);
            return static_cast<int>(nodes.size()) - 1;
        }
    }

    // Comparator helpers
    static bool box_compare(const shared_ptr<hittable>& a,
                            const shared_ptr<hittable>& b,
                            int axis_index) {
        auto a_axis = a->bounding_box().axis_interval(axis_index);
        auto b_axis = b->bounding_box().axis_interval(axis_index);
        return a_axis.min < b_axis.min;
    }

    static bool box_x_compare(const shared_ptr<hittable>& a,
                              const shared_ptr<hittable>& b) {
        return box_compare(a, b, 0);
    }
    static bool box_y_compare(const shared_ptr<hittable>& a,
                              const shared_ptr<hittable>& b) {
        return box_compare(a, b, 1);
    }
    static bool box_z_compare(const shared_ptr<hittable>& a,
                              const shared_ptr<hittable>& b) {
        return box_compare(a, b, 2);
    }
};

#endif