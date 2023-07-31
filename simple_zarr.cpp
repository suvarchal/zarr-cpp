#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>

void create_directory(const std::string& path) {
    if (!std::filesystem::create_directory(path)) {
        std::cout << "Failed to create directory.\n";
    } else {
        std::cout << "Directory created successfully.\n";
    }
}


//rename this to get_path
template<typename... Args>
std::filesystem::path create_path(Args... args) {
    std::filesystem::path result;
    std::filesystem::path arr[] = { args... };
    for(size_t i = 0; i < sizeof...(args); i++) {
        result /= arr[i];
    }
    return result;
}

std::string get_zattrs_json(const std::optional<std::string>& attrs) {
    return attrs.value_or("{}");
}

std::string get_zattrs_json() {
    return get_zattrs_json(std::nullopt);
}

std::string get_zgroup_json() {
    nlohmann::json json_text;

    json_text["zarr_format"] = 2;

    return json_text.dump(); // convert the json structure to a string
}

std::string get_zarray_json(const std::vector<int>& chunks,
                            const std::vector<int>& array_shape,
                            const std::string& dtype,
                            const std::optional<float>& fill_value,
                            const std::optional<std::string>& order,
                            const std::optional<std::string>& compressor,
                            const std::optional<std::string>& filters) {

    nlohmann::json json_text;

    json_text["chunks"] = chunks;
    json_text["shape"] = array_shape;
    json_text["dtype"] = dtype;

    if (fill_value.has_value())
        json_text["fill_value"] = fill_value.value();
    else
        json_text["fill_value"] = nullptr;

    if (order.has_value())
        json_text["order"] = order.value();
    else
        json_text["order"] = "C";

    if (compressor.has_value())
        json_text["compressor"] = compressor.value();
    else
        json_text["compressor"] = nullptr;

    if (filters.has_value())
        json_text["filters"] = filters.value();
    else
        json_text["filters"] = nullptr;

    json_text["zarr_format"] = 2;

    return json_text.dump(); // convert the json structure to a string
}

std::vector<double> generate_random_array(int size) {
    // Initialize a random number generator
    std::default_random_engine generator;
    // Use a distribution that produces values in the range [0.0, 1.0)
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<double> result(size);
    for(int i = 0; i < size; i++) {
        // Generate a random value and add it to the vector
        result[i] = distribution(generator);
    }

    return result;
}

void write_data_to_file(const std::string& path, const std::string* data) {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }
    file << *data;
}

void write_data_to_file(const std::string& path, const void* data, std::size_t size) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }
    file.write(reinterpret_cast<const char*>(data), size);
}

int main(){
//get_zattrs_json({});

std::string dataset_name = "test.zarr";

create_directory(dataset_name);
std::filesystem::path temppath= create_path(dataset_name);

std::filesystem::path zgpath= create_path(temppath.string(), ".zgroup");
std::string zgroup = get_zgroup_json();
write_data_to_file(zgpath, &zgroup);

temppath = create_path(temppath.string(), ".zattrs");
std::string zattrs = get_zattrs_json();
write_data_to_file(temppath.string(), &zattrs);

//create a var
std::filesystem::path vpath = create_path(dataset_name, "sst");
create_directory(vpath.string());

std::filesystem::path zpath = create_path(vpath.string(), ".zattrs");
zattrs = get_zattrs_json("{\"_ARRAY_DIMENSIONS\":[\"nod2d\", \"time\"]}");
write_data_to_file(zpath.string(), &zattrs);

std::filesystem::path zapath = create_path(vpath.string(), ".zarray");
std::vector<int> chunks = {10,1};
std::vector<int> array_shape = {10,1};
std::string dtype = "<f8";
std::optional<float> fill_value; // not provided
std::optional<std::string> order; // not provided
std::optional<std::string> compressor; // not provided
std::optional<std::string> filters; // not provided

std::string zarray = get_zarray_json(chunks, array_shape, dtype, fill_value, order, compressor, filters);
write_data_to_file(zapath.string(), &zarray);
temppath = create_path(vpath.string(), "0");
std::vector data = generate_random_array(10);
//write_data_to_file(temppath.string(), data, sizeof(data));
write_data_to_file(temppath.string(), data.data(), data.size() * sizeof(double));
}
