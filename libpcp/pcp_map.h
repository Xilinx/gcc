// Copyright (C) 2009 Free Software Foundation, Inc.
// Contributed by Jan Sjodin <jan.sjodin@amd.com>.

// This file is part of the Polyhedral Compilation Package Library (libpcp).

// Libpcp is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.

// Libpcp is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
// more details.

// You should have received a copy of the GNU Lesser General Public License 
// along with libpcp; see the file COPYING.LIB.  If not, write to the
// Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.  

// As a special exception, if you link this library with other files, some
// of which are compiled with GCC, to produce an executable, this library
// does not by itself cause the resulting executable to be covered by the
// GNU General Public License.  This exception does not however invalidate
// any other reasons why the executable file might be covered by the GNU
// General Public License.  

#ifndef _PCP_MAP_
#define _PCP_MAP_
#include "pcp_alloc.h"

template <typename Key, typename Value> class PcpMap
{
 public:
  virtual void insert(Key k, Value v) = 0;
  virtual Value lookup(Key k) = 0;
  virtual bool contains(Key k) = 0;
};


// Simple implementation of a map using a (non-balanced) binary tree.
// Assumptions: operators '=', '<' and '>' available

template <typename Key, typename Value> class PcpTreeMap 
: public PcpMap<Key, Value>
{
 protected:
  class Entry
  {
    Key key;
    Value value;
    Entry* left;
    Entry* right;

  public:

    void setKey(Key key)
    {
      this->key = key;
    }

    Key getKey()
    {
      return this->key;
    }

    void setValue(Value value)
    {
      this->value = value;
    }

    Value getValue()
    {
      return this->value;
    }

    void setLeft(Entry* left)
    {
      this->left = left;
    }

    Entry* getLeft()
    {
      return this->left;
    }

    void setRight(Entry* right)
    {
      this->right = right;
    }

    Entry* getRight()
    {
      return this->right;
    }

    Entry(Key key, Value value)
    {
      setKey(key);
      setValue(value);
      setLeft(NULL);
      setRight(NULL);
    }
  };

  Entry* tree;

  void setTree(Entry* tree)
  {
    this->tree = tree;
  }

  Entry* getTree()
  {
    return this->tree;
  }

  virtual void insertTree(Entry* tree, Key key, Value value)
  {
    bool inserted = false;
    while(!inserted)
      {
	Key treeKey = tree->getKey();
	if(treeKey == key)
	  {
	    tree->setValue(value);
	    inserted = true;
	  }
	else if(treeKey > key)
	  {
	    if(tree->getLeft() == NULL)
	      {
		tree->setLeft(new Entry(key, value));
		inserted = true;
	      }
	    else
	      tree = tree->getLeft();
	  }
	else if(tree->getRight() == NULL)
	  {
	    tree->setRight(new Entry(key, value));
	    inserted = true;
	  }
	else
	  tree = tree->getRight();
      }
  }

 public:
  void insert(Key key, Value value)
  {
    if(this->getTree() == NULL)
      {
	setTree(new Entry(key, value));
      }
    else
      {
	insertTree(this->getTree(), key, value);
      }
  }

  virtual Value lookup(Key key)
  {
    Entry* tree = getTree();
    while(tree != NULL)
      {
	Key treeKey = tree->getKey();
	if(treeKey == key)
	  return tree->getValue();
	else 
	  tree = (treeKey > key) ? tree->getLeft() : tree->getRight(); 
      }
  }

  virtual bool contains(Key key)
  {
    Entry* tree = getTree();
    while(tree != NULL)
      {
	Key treeKey = tree->getKey();
	if(treeKey == key)
	  return true;
	else 
	  tree = (treeKey > key) ? tree->getLeft() : tree->getRight(); 
      }
    return false;
  }

  PcpTreeMap()
    {
      setTree(NULL);
    }
};

#endif // _PCP_MAP_ 
