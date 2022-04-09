//
// Created by atimin on 06.05.21.
//

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <sf_compressor/sf_compressor.h>
#include <sf_compressor/utils.h>

using drift::sf::CreateRandomMatrix;
using drift::sf::SfCompressor;

auto print_vec = [](std::string title, auto vector) {
  std::cout << title << ":\n";
  for (auto& v : vector) {
    std::cout << v << ",";
  }
  std::cout << std::endl;
};

[[maybe_unused]] auto save_matrix = [](const std::string& path,
                                       const SfCompressor::OriginalData& data) {
  std::ofstream file(path);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  file << static_cast<int>(data.frag_length) << "," << data.row_based << ","
       << data.rows << "," << data.columns << std::endl;
  for (auto& index : data.indexes) {
    file << index << ",";
  }

  file << std::endl;
  for (auto& value : data.values) {
    file << value << ",";
  }

  file << std::endl;
};

auto load_matrix = [](const std::string& path) -> SfCompressor::OriginalData {
  std::ifstream file(path);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  SfCompressor::OriginalData data;

  std::string line;
  std::getline(file, line);
  auto ss = std::istringstream(line);

  std::string val;
  std::getline(ss, val, ',');
  data.frag_length = std::stoi(val);
  std::getline(ss, val, ',');
  data.row_based = std::stoi(val);
  std::getline(ss, val, ',');
  data.rows = std::stoi(val);
  std::getline(ss, val, ',');
  data.columns = std::stoi(val);

  std::getline(file, line);
  ss = std::istringstream(line);
  for (std::string index; std::getline(ss, index, ',');) {
    data.indexes.push_back(std::stoi(index));
  }

  std::getline(file, line);
  ss = std::istringstream(line);
  for (std::string value; std::getline(ss, value, ',');) {
    data.values.push_back(std::stof(value));
  }

  return data;
};

int main() {
  for (int i = 0; i < 1; ++i) {
    auto in_data = CreateRandomMatrix(1000, 1000, 0.9);
    save_matrix("matrix.csv", in_data);

    SfCompressor compressor1, compressor2;

    std::vector<uint8_t> blob;
    if (!compressor1.Compress(in_data, &blob)) {
      return -1;
    }

    std::cout << "Compressed size=" << blob.size() << " for "
              << in_data.values.size() << " points" << std::endl;

    SfCompressor::OriginalData out_data{};
    if (!compressor1.Decompress(blob, &out_data)) {
      return -1;
    }

    if (!compressor1.Decompress(blob, &out_data)) {
      return -1;
    }

    // print_vec("IN INDEX", in_data.indexes);
    // print_vec("OUT INDEX", out_data.indexes);
    // print_vec("IN VALUES", in_data.values);
    // print_vec("OUT VALUES", out_data.values);

    assert(out_data == in_data);
  }
  return 0;
}