#include <corsika/Index.h>

namespace corsika
{
    namespace detail
    {
        bool less(double i, double j) { return i<j; }
        bool less_equal(double i, double j) { return i<=j; }
        bool greater(double i, double j) { return i>j; }
        bool greater_equal(double i, double j) { return i>=j; }
        
        size_t digitize(const std::vector<double>& arr, double x)
        {
            // my own binary search:
            // if (arr.size() == 0 || x < arr[0]) return 0;
            // unsigned int low = 0;
            // unsigned int high = arr.size()-1;
            // while (low < high-1) {
            // 	const unsigned int i = (low+high)/2;
            // 	if (arr[i] <= x) low = i;
            // 	else high = i;
            // }
            // if (arr[high] <= x) return high+1;
            // if (arr[low] <= x) return low+1;
            // return low;
            
            // the dumbest implementation ever, but it works:
            // if (x<arr[0])
            // 	return 0;
            // for (unsigned int i = 0; i < arr.size(); ++i) {
            // 	if (arr[i] > x) return i;
            // }
            // return arr.size();
            
            return std::upper_bound(arr.begin(), arr.end(), x, less) - arr.begin();
        }
    }
}
