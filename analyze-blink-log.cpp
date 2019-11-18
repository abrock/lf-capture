#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>

#include <runningstats/runningstats.h>

void smooth(std::vector<double> & vec) {
    std::vector<double> res;
    for (size_t ii = 1; ii+1 < vec.size(); ++ii) {
        res.push_back((vec[ii-1] + 2*vec[ii] + vec[ii+1])/4);
    }
    vec = res;
}

void diff(std::vector<double> & vec) {
    std::vector<double> res;
    for (size_t ii = 1; ii < vec.size(); ++ii) {
        res.push_back(vec[ii] - vec[ii-1]);
    }
    vec = res;
}

void smoothdiff(std::vector<double> & vec) {
    smooth(vec);
    smooth(vec);
    smooth(vec);
    smooth(vec);
    smooth(vec);
    smooth(vec);
    diff(vec);
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " logfile" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename(argv[1]);
    std::ifstream in(filename);

    std::vector<double> d_dst, d_src;

    double min_dst = std::numeric_limits<double>::max();
    double min_src = min_dst;
    double max_dst = std::numeric_limits<double>::min();
    double max_src = max_dst;

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream _line(line);
        double dst = 0, src = 0;
        _line >> dst;
        _line >> src;
        d_dst.push_back(dst);
        d_src.push_back(src);
    }

    smoothdiff(d_dst);
    smoothdiff(d_src);

    std::ofstream smoothed(filename + "-smoothed");

    for (size_t ii = 0; ii < d_dst.size(); ++ii) {
        min_dst = std::min(min_dst, d_dst[ii]);
        min_src = std::min(min_src, d_src[ii]);

        max_dst = std::max(max_dst, d_dst[ii]);
        max_src = std::max(max_src, d_src[ii]);
        smoothed << d_dst[ii] << "\t" << d_src[ii] << std::endl;
    }

    double const middle_dst = (min_dst + max_dst)/2;
    double const middle_src = (min_src + max_src)/2;

    double last_dst = d_dst.front();
    double last_src = d_src.front();
    size_t rising_src = 0;
    size_t falling_src = 0;

    runningstats::QuantileStats<double> rising, falling, both;
    for (size_t ii = 0; ii < d_dst.size(); ++ii) {
        if (last_src < middle_src && d_src[ii] >= middle_src) {
            rising_src = ii;
        }
        if (last_src > middle_src && d_src[ii] <= middle_src) {
            falling_src = ii;
        }

        if (last_dst < middle_dst && d_dst[ii] >= middle_dst && rising_src != 0) {
            std::cout << "Rising: " << rising_src << "\t" << ii << "\t" << ii - rising_src << std::endl;
            rising.push(ii - rising_src);
            both.push(ii - rising_src);
        }
        if (last_dst > middle_dst && d_dst[ii] <= middle_dst && falling_src != 0) {
            std::cout << "Falling: " << falling_src << "\t" << ii << "\t" << ii - falling_src << std::endl;
            falling.push(ii - falling_src);
            both.push(ii - falling_src);
        }


        last_dst = d_dst[ii];
        last_src = d_src[ii];
    }

    rising.sort();
    falling.sort();
    both.sort();

    falling.plotHistAndCDF(filename + "-falling", 1, false);
    rising.plotHistAndCDF(filename + "-rising", 1, false);
    both.plotHistAndCDF(filename + "-both", 1, false);

    std::cout << "Rising data: " << std::endl;
    for (double const it : rising.getData()) {
        std::cout << it << std::endl;
    }

    std::cout << "Falling data: " << std::endl;
    for (double const it : falling.getData()) {
        std::cout << it << std::endl;
    }

    std::cout << "Both data: " << std::endl;
    for (double const it : both.getData()) {
        std::cout << it << std::endl;
    }

    std::cout << "Stats: " << std::endl
              << "Rising: " << rising.print() << std::endl
              << "Falling: " << falling.print() << std::endl
              << "Both: " << both.print() << std::endl;


    return EXIT_SUCCESS;
}
