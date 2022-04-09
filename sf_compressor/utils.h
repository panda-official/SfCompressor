// Copyright 2021 PANDA GmbH
#ifndef SRC_SF_UTILS_H_
#define SRC_SF_UTILS_H_

#include <random>
#include <utility>
#include <vector>

#include "sf_compressor/sf_compressor.h"

namespace drift::sf {

SfCompressor::OriginalData CreateRandomMatrix(size_t rows, size_t columns,
                                              float zeros_level,
                                              uint8_t frag_length = 7) {
  const int kResolution = 1024;
  const size_t size = rows * columns;
  std::default_random_engine gen;
  std::uniform_int_distribution<int> data_dist(0, kResolution);
  std::uniform_real_distribution<float> zero_dist(0.f, 1.f);

  std::vector<uint32_t> indexes;
  indexes.reserve(size);
  std::vector<float> values;
  values.reserve(size);
  for (int i = 0; i < size; ++i) {
    if (zero_dist(gen) > zeros_level) {
      indexes.push_back(i);
      values.push_back(static_cast<float>(data_dist(gen)) / kResolution);
    }
  }

  return SfCompressor::OriginalData{
      .frag_length = frag_length,
      .row_based = true,
      .rows = rows,
      .columns = columns,
      .indexes = std::move(indexes),
      .values = std::move(values),
  };
}

}  // namespace drift::sf
#endif  // SRC_SF_UTILS_H_
