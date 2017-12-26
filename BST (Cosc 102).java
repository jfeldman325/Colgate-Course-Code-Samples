import java.util.*;

public class BST
{
    // component node class
    private static class Node
    {
        int key;
        Node left;
        Node right;
        
        Node() { super(); }
        
        Node(int key) { super(); this.key = key; }
    }
    
    // keeps track of root and size
    private Node root;
    private int size;
    
    // default constructor    
    public BST() { super(); }
    
    
    // method to access number of keys in the tree
    public int size()
    {
        return size;
    }
    
    // standard BST insert -- non-balancing
    public boolean insert (int key)
    {
        if (root == null) {
            root = new Node(key);
            size = 1;
            return true;
        }
            
        return insert(root, key);    
    }
    
    // recursive helper for insert
    private boolean insert (Node n, int key)    
    {
        if (n.key == key)
            return false;
        
        if (key > n.key) {
            if (n.right == null) {
                n.right = new Node(key);
                size++;
                return true;
            }
            else
                return insert(n.right, key);
        }
        else {  // key < n.key
            if (n.left == null) {
                n.left = new Node(key);
                size++;
                return true;
            }
            else
                return insert(n.left, key);
        }
    }
    
    // tests if an element is in the tree
    public boolean contains (int key)
    {
        Node n = root;
        while (n != null) {
            if (n.key == key) return true;
            if (key < n.key)
                n = n.left;
            else
                n = n.right;
        }
        
        return false;
    }
    
    // prints a preorder traversal, with X for null pointers
    public void preorder()
    {
        preorder(root);
        System.out.println();
    }
    
    // recursive helper for preorder traversal
    private void preorder(Node root)
    {
        if (root == null) {
            System.out.print("X ");
            return;
        }
            
        System.out.print(root.key);
        System.out.print(' ');
        
        preorder(root.left);
        preorder(root.right);
    }
      


    /************
    
    for testing
    
    ************/
    
    public static void main (String[] args)
    {
           BST tree = new BST();
           tree.insert(10);
           tree.insert(5);
           tree.insert(15);
           tree.insert(3);
           tree.insert(2);
           tree.insert(4);
           tree.insert(6);
           tree.insert(16);
           tree.insert(11);
           //tree.preorder();
           /*--> should print
               10 5 X X 15 X X
             */
       
        int[] arr = {1, 4, 5, 6};
        BST tree2 = new BST(arr);
        //tree2.preorder();
        //--> should print
            //: 5 4 1 X X X 6 X X
        //--> or should print
            //: 4 1 X X 5 X 6 X X
     
       //System.out.println(tree.LCA(2,16));
       //System.out.println(tree.findBetween(2, 15));
       BST tree3 = new BST();
       tree3 = merge(tree, tree2);
       tree3.preorder();
    }
    
    /*************
    
    Complete the methods below
    
    *************/
    
    public int LCA (int x, int y)
    {
      if (!this.contains(x) || !this.contains(y))
        throw new NoSuchElementException();
      Node current = this.root;
      Node LCA = null;
      int i = 0;
      while (!(current.left == null) || !(current.right == null)) {
        if (x > current.key && y > current.key) {
          current = current.right;
          i++;
        }
        else if (x < current.key && y < current.key) {
          current = current.left;
          i++;
        }
        else {
          LCA = current;
          break;
        }
      }
      return LCA.key;
    }
    
    public static ArrayList<Integer> nums = new ArrayList<Integer>();
    
    public ArrayList<Integer> findBetween (int x, int y)
    {
      findHelper(this.root, x, y);
      return nums;
    } 
    
    public static void findHelper(Node temp, int x, int y) {
      if (temp == null) {
        return;
      }
      if (x < temp.key) {
        findHelper(temp.left, x, y);
      }
       if (x < temp.key && y > temp.key) {
        nums.add(temp.key);
      }
       if (y > temp.key) {
        findHelper(temp.right, x, y);
      }
    }
    
    public BST(int[] arr)
    {
      root = BSTHelper(arr, 0, arr.length - 1);
    }
    
    public Node BSTHelper(int[] arr, int beg, int end) {
      if (beg > end) {
        return null;
      }
      int mid = (beg + end)/2;
      Node temp = new Node();
      temp.key = arr[mid];
      temp.left = BSTHelper(arr, beg, mid - 1);
      temp.right = BSTHelper(arr, mid + 1, end);
      return temp;
    }
    
    public static BST merge(BST t1, BST t2)
    {
      int[] a = t1.findRange(t1);
      int[] b = t2.findRange(t2);
      int[] mergedList = mergeHelper(a, b);
      BST tree = new BST(mergedList);
      return tree;
    }
    
    public static int[] mergeHelper(int[] a, int[] b) {
      int[] product = new int[a.length + b.length];
      int i = 0; int j = 0; int k = 0;
      while (i < a.length && j < b.length) {
        if (a[i] < b[j])       
          product[k++] = a[i++];
        else        
          product[k++] = b[j++];               
      }
      while (i < a.length)  
        product[k++] = a[i++];
      while (j < b.length)    
        product[k++] = b[j++];
      return product;
    }

    
    public int[] findRange(BST tree) {
      Node temp = tree.root;
      int max;
      int min;
      while (!(temp.right == null)) {
        temp = temp.right;
      }
      max = temp.key;
      
      Node temp1 = tree.root;
      while (!(temp1.left == null)) {
        temp1 = temp1.left;
      }
      min = temp1.key;
      
      ArrayList<Integer> list = findBetween(0, max+1);
      int[] output = new int[list.size()];
      for (int i = 0; i < list.size(); i++){
        output[i] = list.get(i);
      }
      return output;
    }


     /*different way of calling merge -- you don't have to modify this method at all
     this merges the contents of the other tree into this one */
    public void merge(BST other)
    {
        BST t = merge(this, other);
        this.root = t.root;
        this.size = t.size;
        t = null;
    }
}