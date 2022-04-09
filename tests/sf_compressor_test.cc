// Copyright 2021 PANDA GmbH
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include <sf_compressor/sf_compressor.h>
#include <sf_compressor/utils.h>

using drift::sf::CreateRandomMatrix;
using drift::sf::SfCompressor;

TEST_CASE("Tests sf::SfCompressor") {
  SfCompressor compressor(3'000'000);
  std::vector<uint8_t> blob;

  SECTION("check comparison") {
    SfCompressor::OriginalData data1 = {
        .frag_length = 7,
        .row_based = true,
        .rows = 10,
        .columns = 15,
        .indexes = {1, 3, 5},
        .values = {1.f, 3.f, -1.f},
    };

    SfCompressor::OriginalData data2 = data1;
    REQUIRE(data1 == data2);

    data2.values[0] -= 0.05f;
    REQUIRE_FALSE(data1 == data2);
  }

  SECTION("should compress") {
    SfCompressor::OriginalData data = {
        .frag_length = 7,
        .row_based = true,
        .rows = 10,
        .columns = 15,
        .indexes = {1, 3, 5},
        .values = {1.f, 3.f, -1.f},
    };

    const auto kExpectedBlob =
        std::vector<uint8_t>{36, 175, 209, 1, 13, 240, 7, 64, 208, 0};
    REQUIRE(compressor.Compress(data, &blob));
    REQUIRE(blob == kExpectedBlob);

    SECTION("repeatably") {
      REQUIRE(compressor.Compress(data, &blob));
      REQUIRE(blob == kExpectedBlob);
    }
  }

  SECTION("should compress and decompress mat 300x200 with 99% zeros") {
    auto data = CreateRandomMatrix(1000, 1310, 0.99);

    CAPTURE(data.values.size());
    REQUIRE(compressor.Compress(data, &blob));
    //    REQUIRE(blob.size() == 1374);

    SfCompressor::OriginalData restored_data;
    REQUIRE(compressor.Decompress(blob, &restored_data));
    //REQUIRE(restored_data == data);
  }

  SECTION("check all fragments and different sizes") {
    auto frag_length =
        GENERATE(7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23);
    auto sizes = GENERATE(std::make_pair(20, 10), std::make_pair(60, 48),
                          std::make_pair(1920, 1200));

    CAPTURE(frag_length);
    CAPTURE(sizes.first);
    CAPTURE(sizes.second);

    auto data = CreateRandomMatrix(sizes.first, sizes.second, 0.7, frag_length);
    SfCompressor::OriginalData restored_data;

    CAPTURE(data.values.size());

    REQUIRE(compressor.Compress(data, &blob));
    REQUIRE(compressor.Decompress(blob, &restored_data));
    //REQUIRE(restored_data == data);
  }

  SECTION("should check") {
    SECTION("bad size") {
      SfCompressor::OriginalData data = {
          .frag_length = 23,
          .row_based = true,
          .rows = 1,
          .columns = 2,
          .indexes = {1, 3, 5},
          .values = {1.f, 3.f, -1.f},
      };

      REQUIRE_FALSE(compressor.Compress(data, &blob));
    }

    SECTION("size indexes and values mismatch") {
      SfCompressor::OriginalData data = {
          .frag_length = 7,
          .row_based = true,
          .rows = 5,
          .columns = 5,
          .indexes = {1, 5},
          .values = {1.f, 3.f, -1.f},
      };

      REQUIRE_FALSE(compressor.Compress(data, &blob));
    }

    SECTION("fragment length must be 7-21 or 23") {
      REQUIRE_FALSE(compressor.Compress({.frag_length = 6}, &blob));
      REQUIRE_FALSE(compressor.Compress({.frag_length = 22}, &blob));
      REQUIRE_FALSE(compressor.Compress({.frag_length = 24}, &blob));
    }
  }
}
