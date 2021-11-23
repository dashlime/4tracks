#ifndef UTILS_FUNCTIONS_H
#define UTILS_FUNCTIONS_H

#include <iostream>
#include <vector>
#include <math.h>

class Utils {
public:
    static long search_closest(const std::vector<double>& sorted_array, double x) {

        auto iter_geq = std::lower_bound(
            sorted_array.begin(),
            sorted_array.end(),
            x
        );

        if (iter_geq == sorted_array.begin()) {
            return 0;
        }

        double a = *(iter_geq - 1);
        double b = *(iter_geq);

        if (fabs(x - a) < fabs(x - b)) {
            return iter_geq - sorted_array.begin() - 1;
        }

        return iter_geq - sorted_array.begin();

    }
};

#endif // UTILS_FUNCTIONS_H
