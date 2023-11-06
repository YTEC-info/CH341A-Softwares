#include "MathUtils.h"
#include <math.h>
#include <algorithm>
#include <limits>

bool MathUtils::AlmostEqual(double v1, double v2) {
	if (std::fabs(v1) < 1e-10 && std::fabs(v2) < 1e-10) {
		return true;
	}
	return (std::fabs(v1 - v2) < std::fabs(std::min(v1, v2)) * std::numeric_limits<double>::epsilon());
}
