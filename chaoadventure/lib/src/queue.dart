// Making my own queue object, in dart this is way too simple but it provides function symbols specific to a queue
class Queue<T> {
  List<T> queueList = [];

  Queue();

  T? getFirst() {
    if (queueList.isNotEmpty) {
      return queueList[0];
    }
    return null;
  }

  int getSize() {
    return queueList.length;
  }

  void push(T value) {
    queueList.add(value);
  }

  void pushInFront(T value) {
    queueList.insert(0, value);
  }

  void pop() {
    queueList.removeAt(0);
  }
}
