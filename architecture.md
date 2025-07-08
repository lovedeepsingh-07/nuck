# Architecture Guide

Writing a physics engine generally requires a knowlegde of basic concepts of physics but those are also explained in this guide.

The physics engine contains many components and we will be implementing them one-by-one in a specific order:

- slotmap storage
- broadphase and narrowphase collision detection
- AABB: axis aligned bounding box
- separating axis theorem
- spatial partitioning (spatial hashing)
- linear dynamics
- different object types
- sleep optimization
- collision detection with callback API

> NOTE: You don't have to worry about anything in this list, every single one of these things will be deeply and simply explained in this guide

# Introduction

A physics engine as it's name suggests is a system that handles the simulation of real-world physics in computers. They are especially useful in game-development as they offer a solution for simluating real-life physics of playable characters (jumping, shotting, punching etc).

Think of it like this, while writing a game, when your character jumps, first of all you want your character to actually go up, then you want your character to come down under the effect of gravity and you want the jump to be consistent, when your character shoots a gun, you want bullets to fly out and damage the enemy. <br/>
A physics engine handles all that.

How does it work though? Its very simple. The physics engine contains a `World` which in the beginning is just empty(no objects). The world is responsible for storing the objects that you create, apply physical phenomenon such as gravity to those said objects and allow you to manipulate objects easily, and that's it.

# Slotmap Storage

## What is it ?

Slotmap is a data-structure that can be thought of as a more efficient type of list.

To put it simply, we will have a vector say "objects", it will contain all the objects but we will have 2 other vectors alongside it, "free_indexes" and "generations".

- `objects`: contains all the actual physical objects, this vector will only be updated with two actions, replacing the object in an index, and adding an object, we won't do anything else, we will not be deleting anything from this
- `generations`: tracks the generation of object in a specific index, basically this ensures proper deletion of objects, lets say we have an empty world with no objects, when you create an object, it will get index_0 and generation_0, so basically, we will just put that object at index 0 in the "objects" vector and add 0 to the same index 0 in the "generations" vector, now say we delete that object, what will happen is that we will just take the the object's generation and index, make sure the object matches the generation value in that index in "generations" vector, then add 1 to that index in "generations" vector, and then just push that index of the object(in our case 0) to the "free_indexes" vector because that index is now free, and that's it, we don't have to actually erase/delete anything from "objects" vector
- `free_indexes`: tracks all the indexes that are free in the "objects" vector, after deleting an object in the previous step, we will have now 0 added to the free_indexes vector, now if we try to create an object, instead of just pushing that object to the "objects" vector, what it will do is get the last index value from "free_indexes", pop the "free_indexes" vector from the back, now the index we will have is the index from where an object was deleted previously(the previous object is still there but not in our record anymore, the index of that object is basically free), and then we will just place the new object at that index, we will get the generation value of that index from "generation" vector and set the generation for that object equal to that value.

In order to properly access the objects, we will utilize "ObjectID"s. They are basically [plain old data types (POD)](https://en.wikipedia.org/wiki/Passive_data_structure) that just contain the `index` and `generation` of an object, and using those two values alone, we can get any object from our world without having to store the pointer to that object. Because they are PODs, we can easily treat them as components in an ECS architecture.

So, when we create an object, instead of getting that object, we will get the ObjectID which we can use to get that object, and while deleting an object, we will just need to provide the ObjectID.

## Why slotmap ?

Let's say I give you a task of storing a bunch of objects in a physical world and then write functionality to easily lookup those objects, how would you write it? The same thought would pop up in every beginner's mind: "lists". Make a single vector/list and store the bodies in that vector and just loop them up whenever you neeed them. There are a few problems with this approach, notably:

- Lets say you delete the object from array with index 4, all the objects after that index will have their index shifted by one in order to accomodate for the missing element in the array, so now if you have some code that points to element with index 10 in the array, it would now be pointing to the element that previously had the index of 11, and in order to fix that, you have to write code to manually update the referring index of your code or instead of erasing, just mark the deleted element as null, but that would also cause performance issues as your world grows
- You have to write additional code to ensure the consistency of creationg/deletion of objects which can result in performance sacrifices
- This method is extremely prone to dangling pointers and memory leaks as you will be giving direct pointer of objects from the array to your code, it's the same as the index issue, say you have a pointer to an element in the array, but that element was deleted by some other section of your code, now you have a pointer that points to "nothing", this is extremely dangerous.
- This approach will not work with ECS as if you want to write a physics component, it is not possible unless the members of that component are all safe to copy and move around freely
  > [ECS(Entity-Component-System)](https://www.youtube.com/watch?v=2rW7ALyHaas) is an architecture for making games that involves taking a data-oriented approach instead of a object-oriented one

In order to fix all of these issues, we will utilize the same approach as done by [Box2D](https://box2d.org/), we will use the slotmap data-structure.

## How to implement it ?

Now that we have a somewhat intuitive understanding of how this system works, we can move onto writing code for it.

```cpp
    ObjectType type;
    math::Vector2 position;
    math::Vector2 size;
    math::Vector2 velocity;
    math::Vector2 force;
    float mass;
};

struct ObjectID {
    uint32_t index;
    uint32_t generation;
};

class World {
  private:
    std::vector<Object> m_objects;
    std::vector<uint32_t> m_generations;
    std::vector<uint32_t> m_free_indexes;
  public:
    auto create_object() -> ObjectID;
    auto get_object(ObjectID object_id) -> Object *;
    auto destroy_object(ObjectID object_id) -> void;
}
```

As you can see we have added the required vectors and methods to our `World` class.

### create_object

The `create_object` method works as follows: currently it does not take any arguments, but when we add more properties to the Object, it will accept those properties,

- we innitialize an `curr_index` and `curr_gen` variables
- check if there are any free indexes available
- if NO
  - we set the curr_index equal to the total number of elements in the "m_objects" vector, this is because we will pushing a new element onto the "objects" vector, the index of current last element in the vector will be `m_objects.size() - 1` so after pushing an element, the index of that element will be equal to the size of the vector when that element was not added to the vector
  - we push the new object into the "m_objects" vector
  - since its a new element into the objects vector, it will belong to the 0th generation and hence we set the curr_gen equal to zero, then we push curr_gen into the generations vector, the index of this curr_gen in the generations vector and the index of newly added object in the objects vector would automatically be equal
- if YES
  - get the last value of free index from the free_indexes vector and pop that element, and then set the curr_index equal to that free index
  - set the object at the curr_index in the objects vector equal to the new object, so basically we are replacing the older object with the newer one
  - now since we had a free index, the generation of that index in the generations vector would have already been updated, so we just need to get the current generation of that index from the generations vector and set curr_gen equal to that generation
- now we just return the "ObjectID" with corresponding index and generation

```cpp
ObjectID World::create_object(math::Vector2 position, math::Vector2 size, float mass) {
    uint32_t curr_index = 0;
    uint32_t curr_gen = 0;

    if (m_free_indexes.empty()) {
        curr_index = m_objects.size();
        m_objects.push_back(Object{
            .position = position, .size = size, .mass = mass });
        curr_gen = 0;
        m_generations.push_back(curr_gen);
    } else {
        curr_index = m_free_indexes.back();
        m_free_indexes.pop_back();
        m_objects[curr_index] =
            Object{ .position = position, .size = size, .mass = mass };
        curr_gen = m_generations[curr_index];
    }
    return ObjectID{ .index = curr_index, .generation = curr_gen };
}
```

### get_object

The `get_object` method works as follows:

- it takes in the ObjectD as an input
- checks if the index of that id is less than the size of objects vector and generation in generations vector at that index matches the generation of the ID, this ensure that the input ID is valid
- if the id is valid, then we just return the pointer to the object at that index in the objects vector
- if the id is not valid, we just return `nullptr`

```cpp
Object* World::get_object(ObjectID object_id) {
    if (object_id.index < m_objects.size()
        && m_generations[object_id.index] == object_id.generation) {
        return &m_objects[object_id.index];
    }
    return nullptr;
}
```

### destroy_object

the `destroy_object` works as follows:

- it takes in the ObjectID as input
- checks whether the id is still valid the same way we did in the create_object method
- if the id is valid, then we just increment the generation at that index in the generations vector by 1 and push that index onto the free_indexes vector to indicate that the object at that index has been deleted from the world

```cpp
void World::destroy_object(ObjectID object_id) {
    if (object_id.index < m_objects.size()
        && m_generations[object_id.index] == object_id.generation) {
        m_generations[object_id.index] += 1;
        m_free_indexes.push_back(object_id.index);
    }
};
```

### Iterator

In the following guide, we will be adding functionality that applies to all of the objects a given world, in order to make that simpler and easier to do, we will implement a custom iterator for the World class. It can be done by adding the following code to the World class:

> NOTE: I will not be explaining this section because it is strictly C++ specfic and you can figure this out from other C++ sources

```cpp
class World {
  // .. private section
  public:
    struct Iterator {
      private:
        World *m_world;
        size_t m_index;

        void advance_to_next_valid() {
            while (m_index < m_world->m_objects.size()
                   && std::ranges::find(m_world->m_free_indexes, m_index)
                       != m_world->m_free_indexes.end()) {
                m_index = m_index + 1;
            }
        }

      public:
        Iterator(World *world, size_t start) : m_world(world), m_index(start) {
            advance_to_next_valid();
        }

        bool operator!=(const Iterator &other) const {
            return m_index != other.m_index;
        }
        auto operator++() -> Iterator & {
            ++m_index;
            advance_to_next_valid();
            return *this;
        }

        auto operator*() -> std::pair<ObjectID, Object &> {
            return { ObjectID{ static_cast<uint32_t>(m_index), m_world->m_generations[m_index] },
                     m_world->m_objects[m_index] };
        }
    };

    auto begin() -> Iterator {
        return Iterator(this, 0);
    }

    auto end() -> Iterator {
        return Iterator(this, m_objects.size());
    }
    // ... rest of the methods
};
```
