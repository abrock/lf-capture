#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>

#include <runningstats/runningstats.h>

#include <ceres/ceres.h>

struct FitCost {
    double const time;
    double const data;

    FitCost(double const _time, double const _data) : time(_time), data(_data) {}

    template<class T, class U>
    static T function(
            U const time,
            T const& amplitude,
            T const& freq,
            T const& time_offset,
            T const& linear,
            T const& offset
            ) {
        return offset
                + amplitude * ceres::sin(T(time) * freq * T(2*M_PI) + time_offset)
                + T(time) * linear;
    }

    template<class T>
    bool operator () (
            const T * const amplitude,
            const T * const freq,
            const T * const time_offset,
            const T * const linear,
            const T * const offset,
            T * residual
            ) const {

        residual[0] = T(data) - function(time, amplitude[0], freq[0], time_offset[0], linear[0], offset[0]);

        return true;
    }
};

void analyzeCeres(std::vector<double> const& data, double const frequency) {
    ceres::Problem problem;

    double amplitude = 1, freq = 100, time_offset = 0, linear_term = 0, offset = 128;

    for (size_t ii = 0; ii < data.size(); ++ii) {
        double const time = double(ii) / frequency;
        ceres::CostFunction * cost_function =
                new ceres::AutoDiffCostFunction<
                FitCost,
                1, // number of residuals
                1, // amplitude
                1, // freq
                1, // time_offset
                1, // linear term
                1 // offset

                >(new FitCost(time, data[ii]));
        problem.AddResidualBlock(cost_function,
                                 nullptr, // Loss function (nullptr = L2)
                                 &amplitude,
                                 &freq,
                                 &time_offset,
                                 &linear_term,
                                 &offset
                                 );
    }

    double const ceres_tolerance = 1e-16;

    ceres::Solver::Options options;
    options.max_num_iterations = 150;
    options.function_tolerance = ceres_tolerance;
    options.gradient_tolerance = ceres_tolerance;
    options.parameter_tolerance = ceres_tolerance;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    std::cout << summary.FullReport();

    std::cout << "Amplitude: " << amplitude << std::endl
              << "Frequency: " << freq << std::endl
              << "Time offset: " << time_offset << std::endl
              << "Linear term: " << linear_term << std::endl
              << "Offset (~mean value): " << offset << std::endl;

}

void analyzeSingleSub(std::vector<double> const& data, double& mean, double& flicker, double& index) {
    runningstats::RunningStats stats;

    stats.push(data);

    double areaHigh = 0, areaLow = 0;
    mean = stats.getMean();
    for (const auto val : data) {
        if (val > mean) {
            areaHigh += val - mean;
            areaLow += mean;
        }
        else {
            areaLow += val;
        }
    }

    flicker = 100.0 * (stats.getMax() - stats.getMin()) / (stats.getMax() + stats.getMin());

    index = areaHigh / (areaLow + areaHigh);

}

void analyzeSingle(std::vector<double> const& data, double const frequency) {
    double mean = 0, flicker = 0, index = 0;

    analyzeSingleSub(data, mean, flicker, index);

    std::cout << "Evaluation of complete data at once: " << std::endl
              << "Mean value: " << mean << std::endl
              << "Flicker: " << flicker << "%" << std::endl
              << "Index: " << index << std::endl;

    int const sub_size = int(std::ceil(frequency/10));

    runningstats::RunningStats statsMean, statsFlicker, statsIndex;

    for(int ii = 0; ii + sub_size < int(data.size()); ii += sub_size) {
        std::vector<double> sub(data.begin() + ii, data.begin() + ii + sub_size);
        analyzeSingleSub(sub, mean, flicker, index);
        statsMean.push(mean);
        statsFlicker.push(flicker);
        statsIndex.push(index);
    }

    std::cout << "Stats for 1/10s segments:" << std::endl
              << "Mean: " << statsMean.print() << std::endl
              << "Flicker: " << statsFlicker.print() << std::endl
              << "Index: " << statsIndex.print() << std::endl;
}

void analyze(std::vector<std::vector<double> > const& data, size_t const col, double const frequency) {
    std::cout << "Analyzing column #" << col << std::endl;
    double previous = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> local_data;
    for (const auto& line : data) {
        double current = previous;
        if (line.size() > col) {
            current = line[col];
        }
        if (std::isfinite(current)) {
            local_data.push_back(current);
        }
    }
    analyzeSingle(local_data, frequency);

    analyzeCeres(local_data, frequency);

    std::cout << std::endl;
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <luminosity log file> <input frequency = 1000 [Hz]>" << std::endl;
        return EXIT_FAILURE;
    }

    double frequency = 1000;

    if (argc > 2) {
        frequency = std::stod(argv[3]);
    }

    std::ifstream in(argv[1]);

    std::vector<std::vector<double> > data;

    std::string _line;
    size_t max_width = 0;
    while (std::getline(in, _line)) {
        std::stringstream line(_line);

        std::vector<double> line_data;
        while (line) {
            double val = std::numeric_limits<double>::quiet_NaN();
            line >> val;
            if (std::isfinite(val)) {
                line_data.push_back(val);
            }
        }
        data.push_back(line_data);
        max_width = std::max(max_width, line_data.size());
    }

    for (size_t ii = 0; ii < max_width; ++ii) {
        analyze(data, ii, frequency);
    }

}
