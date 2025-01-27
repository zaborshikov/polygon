#include <vector>
#include <algorithm>

class Solution {
public:
    int maxSatisfaction(const vector<int>& satisfaction) {
        std::vector<int> positive;
        std::vector<int> negative;

        for (int elem : satisfaction) {
            if (elem >= 0) {
                positive.push_back(elem);
            } else {
                negative.push_back(elem);
            }
        }

        std::sort(positive.begin(), positive.end());
        std::sort(negative.rbegin(), negative.rend());

        int elem_sum = 0;
        int positive_sum = 0;

        for (int i = 0; i < positive.size(); i++) {
            elem_sum += positive[i] * (i + 1);
            positive_sum += positive[i];
        }

        int bias = 0;
        int elem_sum_neg = 0;
        
        for (int elem : negative) {
            elem_sum_neg += elem;
            if (positive_sum > -1 * elem_sum_neg) {
                bias += (elem_sum_neg + positive_sum);
            } else {
                break;
            }
        }

        return elem_sum + bias;
    }
};
