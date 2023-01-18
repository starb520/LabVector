/***********************************************************************
 * Header:
 *    VECTOR
 * Summary:
 *    Our custom implementation of std::vector
 *      __      __     _______        __
 *     /  |    /  |   |  _____|   _  / /
 *     `| |    `| |   | |____    (_)/ /
 *      | |     | |   '_.____''.   / / _
 *     _| |_   _| |_  | \____) |  / / (_)
 *    |_____| |_____|  \______.' /_/
 *
 *    This will contain the class definition of:
 *        vector                 : A class that represents a Vector
 *        vector::iterator       : An iterator through Vector
 * Author
 *    <your names here>
 ************************************************************************/

#pragma once

#include <cassert>  // because I am paranoid
#include <new>      // std::bad_alloc
#include <memory>   // for std::allocator

class TestVector; // forward declaration for unit tests
class TestStack;
class TestPQueue;
class TestHash;

namespace custom
{

/*****************************************
 * VECTOR
 * Just like the std :: vector <T> class
 ****************************************/
template <typename T, typename A = std::allocator<T>>
class vector
{
   friend class ::TestVector; // give unit tests access to the privates
   friend class ::TestStack;
   friend class ::TestPQueue;
   friend class ::TestHash;
public:
   
   //
   // Construct
   //
   vector(const A& a = A()) :
      data(nullptr), numElements(0), numCapacity(0), alloc(a) {}
   vector(size_t numElements,                const A & a = A());
   vector(size_t numElements, const T & t,   const A & a = A());
   vector(const std::initializer_list<T>& l, const A & a = A());
   vector(const vector &  rhs);
   vector(      vector && rhs);
  ~vector();

   //
   // Assign
   //
   void swap(vector& rhs)
   {
      std::swap(data, rhs.data);
      std::swap(numElements, rhs.numElements);
      std::swap(numCapacity, rhs.numCapacity);
   }

   vector & operator = (const vector & rhs);
   vector& operator = (vector&& rhs)
   {
      clear();
      shrink_to_fit();
      swap(rhs);
      return *this;
   }

   //
   // Iterator
   //

   class iterator;
   iterator begin() { return iterator(data);              }
   iterator end() { return iterator(data + numElements); }

   //
   // Access
   //
         T& operator [] (size_t index);
   const T& operator [] (size_t index) const;
         T& front();
   const T& front() const;
         T& back();
   const T& back() const;

   //
   // Insert
   //
   void push_back(const T& t);
   void push_back(T&& t);
   void reserve(size_t newCapacity);
   void resize(size_t newElements);
   void resize(size_t newElements, const T& t);

   //
   // Remove
   //
   void clear()
   {
      for (size_t i = 0; i < numElements; i++)
         alloc.destroy(&data[i]);
      numElements = 0;
   }

   void pop_back()
   {
      if (numElements)
         alloc.destroy(&data[--numElements]);
   }

   void shrink_to_fit();

   //
   // Status
   //
   size_t  size()          const { return numElements;}
   size_t  capacity()      const { return numCapacity;}
   bool empty()            const { return numElements == 0;}
  
private:
   
   A    alloc;                // use allocator for memory allocation
   T *  data;                 // user data, a dynamically-allocated array
   size_t  numCapacity;       // the capacity of the array
   size_t  numElements;       // the number of items currently used
};

/**************************************************
 * VECTOR ITERATOR
 * An iterator through vector.  You only need to
 * support the following:
 *   1. Constructors (default and copy)
 *   2. Not equals operator
 *   3. Increment (prefix and postfix)
 *   4. Dereference
 * This particular iterator is a bi-directional meaning
 * that ++ and -- both work.  Not all iterators are that way.
 *************************************************/
template <typename T, typename A>
class vector <T, A> ::iterator
{
   friend class ::TestVector; // give unit tests access to the privates
   friend class ::TestStack;
   friend class ::TestPQueue;
   friend class ::TestHash;
public:
   // constructors, destructors, and assignment operator
   iterator() : p(nullptr)              {                           }
   iterator(T* p) : p(p)                {                           }
   iterator(const iterator& rhs)        { this->p = rhs;            }
   iterator(size_t index, vector<T>& v) { this->p = v.data + index; }
   iterator& operator = (const iterator& rhs)
   {
      this->p = rhs.p;
      return *this;
   }

   // equals, not equals operator
   bool operator != (const iterator& rhs) const { return p != rhs.p; }
   bool operator == (const iterator& rhs) const { return p == rhs.p; }

   // dereference operator
   T& operator * ()
   {
      if (p)
         return *p;
      else
         throw "ERROR: Trying to dereference a NULL pointer";
   }

   // prefix increment
   iterator& operator ++ ()
   {
      p++;
      return *this;
   }

   // postfix increment
   iterator operator ++ (int postfix)
   {
      iterator temp = *this;
      p++;
      return temp;
   }

   // prefix decrement
   iterator& operator -- ()
   {
      p--;
      return *this;
   }

   // postfix decrement
   iterator operator -- (int postfix)
   {
      iterator temp = *this;
      p--;
      return temp;
   }

private:
   T* p;
};


/*****************************************
 * VECTOR :: NON-DEFAULT constructors
 * non-default constructor: set the number of elements,
 * construct each element, and copy the values over
 ****************************************/
//template <typename T, typename A>
//vector <T, A> :: vector(const A & a)
//{
//   data = nullptr;
//   numElements = 0;
//   numCapacity = 0;
//   alloc(a);
//}


/*****************************************
 * VECTOR :: NON-DEFAULT constructors
 * non-default constructor: set the number of elements,
 * construct each element, and copy the values over
 ****************************************/
template <typename T, typename A>
vector <T, A> :: vector(size_t num, const T & t, const A & a) : data(nullptr), numElements(0), numCapacity(0), alloc(a)
{
   if (num > 0)
   {
      // allocate memory
      data = alloc.allocate(num);
      numCapacity = num;

      // copy the value
      std::uninitialized_fill_n(data, num, t);
      numElements = num;
   }
}

/*****************************************
 * VECTOR :: INITIALIZATION LIST constructors
 * Create a vector with an initialization list.
 ****************************************/
template <typename T, typename A>
vector <T, A> :: vector(const std::initializer_list<T> & l, const A & a) : data(nullptr), numElements(0), numCapacity(0), alloc(a)
{
   if (l.size())
   {
      // allocate memory
      data = alloc.allocate(l.size());
      numCapacity = l.size();

      int i = 0;
      for (auto it = l.begin(); it != l.end(); ++it)
         alloc.construct(&data[i++], *it);

      assert(i == l.size());
      numElements = l.size();
   }
}

/*****************************************
 * VECTOR :: NON-DEFAULT constructors
 * non-default constructor: set the number of elements,
 * construct each element, and copy the values over
 ****************************************/
template <typename T, typename A>
vector <T, A> :: vector(size_t num, const A & a) :
   data(nullptr), numElements(0), numCapacity(0), alloc(a)
{
   if (num > 0)
   {
      // allocate memory
      data = alloc.allocate(num);
      numCapacity = num;

      // copy the value
      for (size_t i = 0; i < num; i++)
         new (&data[i]) T;
      numElements = num;
   } 
}

/*****************************************
 * VECTOR :: COPY CONSTRUCTOR
 * Allocate the space for numElements and
 * call the copy constructor on each element
 ****************************************/
template <typename T, typename A>
vector <T, A> :: vector (const vector & rhs) : data(nullptr), numElements(0), numCapacity(0), alloc(rhs.alloc)
{
   if (!rhs.empty())
   {
      // allocate memory
      data = alloc.allocate(rhs.numElements);
      numCapacity = rhs.numElements;

   for (size_t i = 0; i < rhs.numElements; i++)
      alloc.construct(&data[i]);
   numElements = rhs.numElements;
   }
}
   
/*****************************************
 * VECTOR :: MOVE CONSTRUCTOR
 * Steal the values from the RHS and set it to zero.
 ****************************************/
template <typename T, typename A>
vector <T, A> :: vector (vector && rhs) : alloc(rhs.alloc)
{
   // copy the array
   data = rhs.data;
   rhs.data = nullptr;

   // copy the number of elements
   numElements = rhs.numElements;
   rhs.numElements = 0;

   // copy the capacity
   numCapacity = rhs.numCapacity;
   rhs.numCapacity = 0;
}

/*****************************************
 * VECTOR :: DESTRUCTOR
 * Call the destructor for each element from 0..numElements
 * and then free the memory
 ****************************************/
template <typename T, typename A>
vector <T, A> :: ~vector()
{
   if (numCapacity != 0)
   {
      assert(nullptr != data);
      // first call the destructor to all the elements from 0..numElements
      for (size_t i = 0; i < numElements; i++)
         alloc.destroy(&data[i]);

      // then free the memory
      alloc.deallocate(data, numCapacity);
   }
}

/***************************************
 * VECTOR :: RESIZE
 * This method will adjust the size to newElements.
 * This will either grow or shrink newElements.
 *     INPUT  : newCapacity the size of the new buffer
 *     OUTPUT :
 **************************************/
template <typename T, typename A>
void vector <T, A> :: resize(size_t newElements)
{
   assert(newElements >= 0);

   // shrink if necessary
   if (newElements < numElements)
   {
      // destroy those off the end
      for (size_t i = newElements; i < numElements; i++)
         alloc.destroy(&data[i]);
   }
   // grow as necessary
   else if (newElements > numCapacity)
   {
      // increase capacity as necessary
      if (newElements > numCapacity)
         reserve(newElements);

      // now fill the new slots with the default T
      for (size_t i = numElements; i < newElements; i++)
      {
         alloc.construct(&data[i]);
         new (&data[i]) T;
      }

      // if we made it this far, adjust the number of elements
      numElements = newElements;
   }
}

template <typename T, typename A>
void vector <T, A> :: resize(size_t newElements, const T & t)
{
   assert(newElements >= 0);

   // shrink as necessary
   if (newElements < numElements)
   {
      // destroy those off the end
      for (size_t i = newElements; i < numElements; i++)
         alloc.destroy(&data[i]);
   }

   // frow as necessary
   else if (newElements > numElements)
   {
      // increase the capacity as necessary
      if (newElements > numCapacity)
         reserve(newElements);

      // now fill the new slots with the default T
      for (size_t i = numElements; i < newElements; i++)
         alloc.construct(&data[i], t);
   }

   // if we have made it this far, adjust the number of elements
   numElements = newElements;
}

/***************************************
 * VECTOR :: RESERVE
 * This method will grow the current buffer
 * to newCapacity.  It will also copy all
 * the data from the old buffer into the new
 *     INPUT  : newCapacity the size of the new buffer
 *     OUTPUT :
 **************************************/
template <typename T, typename A>
void vector <T, A> :: reserve(size_t newCapacity)
{
   // do nothing if we are already big enough
   if (newCapacity <= numCapacity)
      return;
   assert(newCapacity > 0 && newCapacity > numCapacity);

   // allocate the new array
   T* pNew = alloc.allocate(newCapacity);

   // copy over the data from the old array
   for (size_t i = 0; i < numElements; i++)
      new ((void*)(pNew + 1)) T(std::move(data[i]));

   // delete the old and assign the new
   if (nullptr != data)
   {
      // call the destructor on the elements being destroyed
      for (size_t i = 0; i < numElements; i++)
         alloc.destroy(&data[i]);

      alloc.deallocate(data, numCapacity);
   }

   data = pNew;
   numCapacity = newCapacity;
}

/***************************************
 * VECTOR :: SHRINK TO FIT
 * Get rid of any extra capacity
 *     INPUT  :
 *     OUTPUT :
 **************************************/
template <typename T, typename A>
void vector <T, A> :: shrink_to_fit()
{
   // do nothing if we have no space
   if (numCapacity == numElements)
      return;

   // allocate the new array
   T* pNew;
   if (numElements != 0)
   {
      pNew = alloc.allocate(numElements);
      std::uninitialized_copy(data, data + numElements, pNew);
   }
   else
      pNew = nullptr;

   // delete the old and assign the new
   if (nullptr != data)
   {
      // call the destructor on the elements being destroyed
      for (size_t i = 0; i < numElements; i++)
         alloc.destroy(&data[i]);
      // delete the memory
      alloc.deallocate(data, numCapacity);
   }
   data = pNew;
   numCapacity = numElements;
}

/*****************************************
 * VECTOR :: SUBSCRIPT
 * Read-Write access
 ****************************************/
template <typename T, typename A>
T & vector <T, A> :: operator [] (size_t index)
{
   return data[index];

}

/******************************************
 * VECTOR :: SUBSCRIPT
 * Read-Write access
 *****************************************/
template <typename T, typename A>
const T & vector <T, A> :: operator [] (size_t index) const
{
   return data[index];
}

/*****************************************
 * VECTOR :: FRONT
 * Read-Write access
 ****************************************/
template <typename T, typename A>
T & vector <T, A> :: front ()
{
   return data[0];
}

/******************************************
 * VECTOR :: FRONT
 * Read-Write access
 *****************************************/
template <typename T, typename A>
const T & vector <T, A> :: front () const
{
   return data[0];
}

/*****************************************
 * VECTOR :: BACK
 * Read-Write access
 ****************************************/
template <typename T, typename A>
T & vector <T, A> :: back()
{
   return data[numElements - 1];
}

/******************************************
 * VECTOR :: BACK
 * Read-Write access
 *****************************************/
template <typename T, typename A>
const T & vector <T, A> :: back() const
{
   return data[numElements - 1];
}

/***************************************
 * VECTOR :: PUSH BACK
 * This method will add the element 't' to the
 * end of the current buffer.  It will also grow
 * the buffer as needed to accomodate the new element
 *     INPUT  : 't' the new element to be added
 *     OUTPUT : *this
 **************************************/
template <typename T, typename A>
void vector <T, A> :: push_back (const T & t)
{
   if (numCapacity == 0)
      reserve(1);
   else if (numElements == numCapacity)
      reserve(numCapacity * 2);

   new((void*)(data + numElements++)) T(t);
}

template <typename T, typename A>
void vector <T, A> ::push_back(T && t)
{
   if (numCapacity == 0)
      reserve(1);
   else if (numElements == numCapacity)
      reserve(numCapacity * 2);

   new((void*)(data + numElements++)) T(std::move(t));
}

/***************************************
 * VECTOR :: ASSIGNMENT
 * This operator will copy the contents of the
 * rhs onto *this, growing the buffer as needed
 *     INPUT  : rhs the vector to copy from
 *     OUTPUT : *this
 **************************************/
template <typename T, typename A>
vector <T, A> & vector <T, A> :: operator = (const vector & rhs)
{
   // if both sides are same size
   if (rhs.numElements == numElements)
   {
      // no allocator required, just call the assignment operator
      for (size_t i = 0; i < rhs.numElements; i++)
         data[i] = rhs.data[i];
   }

   // if rhs is bigger
   else if (rhs.numElements > numElements)
   {
      // if there is sufficient capacity, but we just need to construct
      if (rhs.numElements <= numCapacity)
      {
         // copy into the already filled slots
         for (size_t i = 0; i < rhs.numElements; i++)
            data[i] = rhs.data[i];

         // copy-construct the rest
         for (size_t i = numElements; i < rhs.numElements; i++)
            alloc.construct(&data[i], rhs.data[i]);
         numElements = rhs.numElements;
      }

      // if there is not sufficient room and we need to grow
      else
      {
         // create enough room
         T* pNew = alloc.allocate(rhs.numElements);

         // copy-construct the existing elements
         for (size_t i = 0; i < rhs.numElements; i++)
            alloc.construct(&pNew[i], rhs.data[i]);

         // get rid of old buffer
         clear();

         alloc.deallocate(data, numCapacity);

         // set up the new member variables
         data = pNew;
         numElements = rhs.numElements;
         numCapacity = rhs.numElements;
      }
   }
   else
   {
      // copy over the elements
      for (size_t i = 0; i < rhs.numElements; i++)
         data[i] = rhs.data[i];

      // now destroy the other spots so they are not constructed
      for (size_t i = rhs.numElements; i < numElements; i++)
         alloc.destroy(&data[i]);
      numElements = rhs.numElements;
   }

   // return self
   return *this;
}

//template <typename T, typename A>
//vector <T, A>& vector <T, A> :: operator = (vector&& rhs)
//{
//   data = rhs.data;
//   rhs.data = nullptr;
//   numElements = rhs.numElements;
//   rhs.numElements = 0;
//   numCapacity = rhs.numCapacity;
//   rhs.numCapacity = 0;
//
//   return *this;
//}




} // namespace custom

