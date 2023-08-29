// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "ppl/cv/arm/boxfilter.h"

#include <chrono>

#include "opencv2/imgproc.hpp"
#include "benchmark/benchmark.h"

#include "ppl/cv/debug.h"
#include "utility/infrastructure.hpp"

using namespace ppl::cv::debug;

template <typename T, int channels, int ksize_x, int ksize_y, ppl::cv::BorderType border_type>
void BM_BoxFilter_ppl_aarch64(benchmark::State &state)
{
    int width = state.range(0);
    int height = state.range(1);
    cv::Mat src = createSourceImage(height, width, CV_MAKETYPE(cv::DataType<T>::depth, channels));
    cv::Mat dst(height, width, CV_MAKETYPE(cv::DataType<T>::depth, channels));

    bool normalize = true;

    int warmup_iters = 5;
    int perf_iters = 50;

    // Warm up the CPU.
    for (int i = 0; i < warmup_iters; i++) {
        ppl::cv::arm::BoxFilter<T, channels>(src.rows,
                                             src.cols,
                                             src.step / sizeof(T),
                                             (T *)src.data,
                                             ksize_x,
                                             ksize_y,
                                             normalize,
                                             dst.step / sizeof(T),
                                             (T *)dst.data,
                                             border_type);
    }

    for (auto _ : state) {
        auto time_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < perf_iters; i++) {
            ppl::cv::arm::BoxFilter<T, channels>(src.rows,
                                                 src.cols,
                                                 src.step / sizeof(T),
                                                 (T *)src.data,
                                                 ksize_x,
                                                 ksize_y,
                                                 normalize,
                                                 dst.step / sizeof(T),
                                                 (T *)dst.data,
                                                 border_type);
        }
        auto time_end = std::chrono::high_resolution_clock::now();
        auto duration = time_end - time_start;
        auto overall_time = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        double time = overall_time * 1.0 / perf_iters;
        state.SetIterationTime(time * 1e-6);
    }
    state.SetItemsProcessed(state.iterations() * 1);
}

#define RUN_PPL_CV_TYPE_FUNCTIONS(type, ksize_x, ksize_y, border_type)                 \
    BENCHMARK_TEMPLATE(BM_BoxFilter_ppl_aarch64, type, c1, ksize_x, ksize_y, border_type) \
        ->Args({640, 480})                                                             \
        ->UseManualTime()                                                              \
        ->Iterations(10);                                                              \
    BENCHMARK_TEMPLATE(BM_BoxFilter_ppl_aarch64, type, c3, ksize_x, ksize_y, border_type) \
        ->Args({640, 480})                                                             \
        ->UseManualTime()                                                              \
        ->Iterations(10);                                                              \
    BENCHMARK_TEMPLATE(BM_BoxFilter_ppl_aarch64, type, c4, ksize_x, ksize_y, border_type) \
        ->Args({640, 480})                                                             \
        ->UseManualTime()                                                              \
        ->Iterations(10);

RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 3, 3, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 3, 3, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 3, 3, ppl::cv::BORDER_REFLECT_101)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 3, 3, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 3, 3, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 3, 3, ppl::cv::BORDER_REFLECT_101)

RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 5, 5, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 5, 5, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 5, 5, ppl::cv::BORDER_REFLECT_101)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 5, 5, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 5, 5, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 5, 5, ppl::cv::BORDER_REFLECT_101)

RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 25, 25, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 25, 25, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 25, 25, ppl::cv::BORDER_REFLECT_101)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 25, 25, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 25, 25, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 25, 25, ppl::cv::BORDER_REFLECT_101)

RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 43, 43, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 43, 43, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(uint8_t, 43, 43, ppl::cv::BORDER_REFLECT_101)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 43, 43, ppl::cv::BORDER_REPLICATE)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 43, 43, ppl::cv::BORDER_REFLECT)
RUN_PPL_CV_TYPE_FUNCTIONS(float, 43, 43, ppl::cv::BORDER_REFLECT_101)

#ifdef PPLCV_BENCHMARK_OPENCV
template <typename T, int channels, int ksize_x, int ksize_y, ppl::cv::BorderType border_type>
void BM_BoxFilter_opencv_aarch64(benchmark::State &state)
{
    int32_t width = state.range(0);
    int32_t height = state.range(1);
    cv::Mat src = createSourceImage(height, width, CV_MAKETYPE(cv::DataType<T>::depth, channels));
    cv::Mat dst(height, width, CV_MAKETYPE(cv::DataType<T>::depth, channels));

    cv::BorderTypes border = cv::BORDER_DEFAULT;
    if (border_type == ppl::cv::BORDER_REPLICATE) {
        border = cv::BORDER_REPLICATE;
    } else if (border_type == ppl::cv::BORDER_REFLECT) {
        border = cv::BORDER_REFLECT;
    } else if (border_type == ppl::cv::BORDER_REFLECT_101) {
        border = cv::BORDER_REFLECT_101;
    } else {
    }

    bool normalize = true;

    int warmup_iters = 5;
    int perf_iters = 50;

    // Warm up the CPU.
    for (int i = 0; i < warmup_iters; i++) {
        cv::boxFilter(src, dst, dst.depth(), cv::Size(ksize_x, ksize_x), cv::Point(-1, -1), normalize, border);
    }

    for (auto _ : state) {
        auto time_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < perf_iters; i++) {
            cv::boxFilter(src, dst, dst.depth(), cv::Size(ksize_x, ksize_x), cv::Point(-1, -1), normalize, border);
        }
        auto time_end = std::chrono::high_resolution_clock::now();
        auto duration = time_end - time_start;
        auto overall_time = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        double time = overall_time * 1.0 / perf_iters;
        state.SetIterationTime(time * 1e-6);
    }
    state.SetItemsProcessed(state.iterations() * 1);
}

#define RUN_OPENCV_TYPE_FUNCTIONS(type, ksize_x, ksize_y, border_type)                       \
    BENCHMARK_TEMPLATE(BM_BoxFilter_opencv_aarch64, type, c1, ksize_x, ksize_y, border_type) \
        ->Args({640, 480})                                                                   \
        ->UseManualTime()                                                                    \
        ->Iterations(10);                                                                    \
    BENCHMARK_TEMPLATE(BM_BoxFilter_opencv_aarch64, type, c3, ksize_x, ksize_y, border_type) \
        ->Args({640, 480})                                                                   \
        ->UseManualTime()                                                                    \
        ->Iterations(10);                                                                    \
    BENCHMARK_TEMPLATE(BM_BoxFilter_opencv_aarch64, type, c4, ksize_x, ksize_y, border_type) \
        ->Args({640, 480})                                                                   \
        ->UseManualTime()                                                                    \
        ->Iterations(10);

RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 3, 3, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 3, 3, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 3, 3, ppl::cv::BORDER_REFLECT_101)
RUN_OPENCV_TYPE_FUNCTIONS(float, 3, 3, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(float, 3, 3, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(float, 3, 3, ppl::cv::BORDER_REFLECT_101)

RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 5, 5, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 5, 5, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 5, 5, ppl::cv::BORDER_REFLECT_101)
RUN_OPENCV_TYPE_FUNCTIONS(float, 5, 5, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(float, 5, 5, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(float, 5, 5, ppl::cv::BORDER_REFLECT_101)

RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 25, 25, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 25, 25, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 25, 25, ppl::cv::BORDER_REFLECT_101)
RUN_OPENCV_TYPE_FUNCTIONS(float, 25, 25, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(float, 25, 25, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(float, 25, 25, ppl::cv::BORDER_REFLECT_101)

RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 43, 43, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 43, 43, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(uint8_t, 43, 43, ppl::cv::BORDER_REFLECT_101)
RUN_OPENCV_TYPE_FUNCTIONS(float, 43, 43, ppl::cv::BORDER_REPLICATE)
RUN_OPENCV_TYPE_FUNCTIONS(float, 43, 43, ppl::cv::BORDER_REFLECT)
RUN_OPENCV_TYPE_FUNCTIONS(float, 43, 43, ppl::cv::BORDER_REFLECT_101)

#endif //! PPLCV_BENCHMARK_OPENCV
