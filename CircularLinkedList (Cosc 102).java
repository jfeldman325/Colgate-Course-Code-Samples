/**
 * SingleLinkedList implements some of the capabilities required by 
 * the List interface using a single linked list data structure.
 * The following methods are provided: get, set, add, remove, size, toString
 * @author Koffman and Wolfgang 
 */
public class CircularLinkedList<E> {
  
  // Nested Class
  /** A Node is the building block for the SingleLinkedList */
  private static class Node<E> {
    
    private E data;                 /** The data value. */
    private Node<E> next = null;    /** The link */
    
    /**
     * Construct a node with the given data value and link
     * @param data - The data value 
     * @param next - The link
     */
    public Node(E data, Node<E> next) {
      this.data = data;
      this.next = next;
    }
    
    /**
     * Construct a node with the given data value
     * @param data - The data value 
     */
//    public Node(E data) {
//      this(data, null);
//    }
  }
  
  private Node<E> tail = null;
  private int size = 0;     /** The size of the list */
  
  /** Insert an item as the first item of the list.
    * @param item The item to be inserted
    */
  private void addFirst(E item) {
    tail = new Node<E>(item, tail);
    size++;
  }
  
  /**
   * Add a node after a given node
   * @param node The node which the new item is inserted after
   * @param item The item to insert
   */
  private void addAfter(Node<E> node, E item) {
    node.next = new Node<E>(item, node.next);
    if (node == tail)
      tail = node.next;
    size++;
  }
  
  /**
   * Append the specified item to the end of the list
   * @param item The item to be appended
   */
  public void add(E item) {
    add(size, item);
    
  }
  
  /**
   * Insert the specified item at the specified position in the list.
   * Shifts the element currently at that position (if any) and any
   * subsequent elements to the right (adds one to their indicies)
   * @param index Index at which the specified item is to be inserted
   * @param item The item to be inserted
   * @throws IndexOutOfBoundsException if the index is out of range
   */
  public void add(int index, E item) {
    if (index < 0 || index > size) 
      throw new IndexOutOfBoundsException(Integer.toString(index));
    
    if (index == 0) 
      addFirst(item);
    else {
      Node<E> node = getNode(index - 1);
      addAfter(node, item); // addAfter will account for checking if the new node is the last node
                            // and updating tail
    }
  }
  
  /**
   * Find the node at a specified index
   * @param index The index of the node sought
   * @returns The node at index or null if it does not exist
   */
  private Node<E> getNode(int index) {
    Node<E> node = tail;
    for (int i = -1; i < index && node != null; i++) 
      node = node.next;
    
    return node;
  }
  
  /**
   * Get the data value at index
   * @param index The index of the element to return
   * @returns The data at index
   * @throws IndexOutOfBoundsException if the index is out of range
   */
  public E get(int index) {
    if (index < 0 || index >= size) 
      throw new IndexOutOfBoundsException(Integer.toString(index));
    
    Node<E> node = getNode(index);
    return node.data;
  }
  
  /**
   * Set the data value at index
   * @param index The index of the item to change
   * @param newValue The new value
   * @returns The data value previously at index
   * @throws IndexOutOfBoundsException if the index is out of range
   */
  public E set(int index, E newValue) {
    if (index < 0 || index >= size) 
      throw new IndexOutOfBoundsException(Integer.toString(index));
    
    Node<E> node = getNode(index);
    E result = node.data;
    node.data = newValue;
    return result;
  }
  
  /**
   * Query the size of the list
   * @return The number of objects in the list
   */
  public int size() {
    return size;
  }
  
  /**
   * Remove the first node from the list
   * @returns The removed node's data or null if the list is empty
   */
  private E removeFirst() {
    Node<E> temp = tail.next;
    tail = tail.next.next;
    size--;
    return temp.data;
  }
  
  /**
   * Remove the node after a given node
   * @param node The node before the one to be removed
   * @returns The data from the removed node, or null
   *          if there is no node to remove
   */
  private E removeAfter(Node<E> node) {
    Node<E> temp = node.next;
    node.next = temp.next;
    size--;
    return temp.data;
  }
  
  /**
   * Remove the item at the specified position in the list. Shifts
   * any subsequent items to the left (subtracts one from their
   * indices). Returns the item that was removed.
   * @param index The index of the item to be removed
   * @returns The item that was at the specified position
   * @throws IndexOutOfBoundsException if the index is out of range
   */
  public E remove(int index) {
    if (index < 0 || index >= size) {
      throw new IndexOutOfBoundsException(Integer.toString(index));}
    if (index == 0)
      return removeFirst();
    else {
      Node<E> a = getNode(index - 1);
      return removeAfter(a);}
  }
      
  /**
   * Remove the first occurrence of element item.
   * @param item The item to be removed
   * @return true if item is found and removed; otherwise, return false.
   */
  public boolean remove(E item) {
    Node<E> n = tail.next;
    for (int i = 0; i < size; i++){
      if (item == n.data){
        n = n.next;
        size--;
        return true;}
      n = n.next;}
    return false;
  }
   
  /**
   * Obtain a string representation of the list
   * @return A String representation of the list 
   */
  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder("[");
    Node<E> p = tail;
    if (p != null) {
      while (p.next != null) {
        sb.append(p.data.toString());
        sb.append(" ==> ");
        p = p.next;
      }
      sb.append(p.data.toString());
    }
    sb.append("]");
    return sb.toString();
  } 
  
  public static void main(String[] args){
    SingleLinkedList<String> classlist = new SingleLinkedList<String>();
    classlist.add("Will");
    classlist.add("Troy");
    classlist.add("Allie");
    System.out.println(classlist.toString());
  }
}
