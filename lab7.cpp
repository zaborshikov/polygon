#include <algorithm> 
class Solution { 
public: 
  int minNumberOperations(vector<int>& target) { 
    target.insert(target.begin(), 0); 
    int s = 0; 
    for (int i = 1; i < target.size(); i++) { 
      s += std::max(0, target[i] - target[i - 1]); 
    } 
    return s; 
  } 
};
