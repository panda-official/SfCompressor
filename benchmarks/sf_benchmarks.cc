// Copyright 2021 PANDA GmbH
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include "sf_compressor/sf_compressor.h"
#include "sf_compressor/utils.h"

using drift::sf::CreateRandomMatrix;
using drift::sf::SfCompressor;

TEST_CASE("Benchmarks sf::SfCompressor") {
  SfCompressor compressor(5'000'000);
  std::vector<uint8_t> blob;
  SfCompressor::OriginalData out_data;

  static const auto kDataWith100Points = CreateRandomMatrix(100, 100, 0.99);
  static const auto kDataWith10KPoints = CreateRandomMatrix(1000, 1000, 0.99);
  static const auto kDataWith1MPoints = CreateRandomMatrix(10000, 10000, 0.99);

  SECTION("Construct") {
    size_t size = GENERATE(1, 100, 10'000, 1'000'000);
    BENCHMARK("Buffer size: " + std::to_string(size)) {
      auto sf = std::make_unique<SfCompressor>(size);
      return sf;
    };
  }

  SECTION("Compress") {
    BENCHMARK("100 Points") {
      auto result = compressor.Compress(kDataWith100Points, &blob);
      return result;
    };

    BENCHMARK("10K Points") {
      auto result = compressor.Compress(kDataWith10KPoints, &blob);
      return result;
    };

    BENCHMARK("1M Points") {
      auto result = compressor.Compress(kDataWith1MPoints, &blob);
      return result;
    };
  }

  SECTION("Decompress") {
    std::vector<uint8_t> blob_100_points, blob_10k_points, blob_1m_points;
    REQUIRE(compressor.Compress(kDataWith100Points, &blob_100_points));
    REQUIRE(compressor.Compress(kDataWith10KPoints, &blob_10k_points));
    REQUIRE(compressor.Compress(kDataWith1MPoints, &blob_1m_points));

    BENCHMARK("100 Points") {
      auto result = compressor.Decompress(blob_100_points, &out_data);
      return result;
    };

    BENCHMARK("10K Points") {
      auto result = compressor.Decompress(blob_10k_points, &out_data);
      return result;
    };

    BENCHMARK("1M Points") {
      auto result = compressor.Decompress(blob_1m_points, &out_data);
      return result;
    };
  }

  SECTION("Compress-Decompress") {
    static const auto kDataWith10KPointsLen7 =
        CreateRandomMatrix(1000, 1000, 0.99, 7);
    static const auto kDataWith10KPointsLen15 =
        CreateRandomMatrix(1000, 1000, 0.99, 15);
    static const auto kDataWith10KPointsLen23 =
        CreateRandomMatrix(1000, 1000, 0.99, 23);

    BENCHMARK("FragLen = 7 (bfloat)") {
      auto ret = compressor.Compress(kDataWith10KPointsLen7, &blob);
      ret = compressor.Decompress(blob, &out_data);
      return ret;
    };

    BENCHMARK("FragLen = 15") {
      auto ret = compressor.Compress(kDataWith10KPointsLen15, &blob);
      ret = compressor.Decompress(blob, &out_data);
      return ret;
    };

    BENCHMARK("FragLen = 23 (float)") {
      auto ret = compressor.Compress(kDataWith10KPointsLen23, &blob);
      ret = compressor.Decompress(blob, &out_data);
      return ret;
    };
  }
}
