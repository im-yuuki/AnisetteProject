//
// Created by Yuuki on 28/03/2025.
//
#pragma once
#include <string>
#include <climits>
#include <functional>
#include <fstream>
#include <unordered_map>

namespace anisette::utils::config
{
    class ConfigValue {
    private:
        const std::string _key;
		const bool _optional, _is_int;

        // This is only used if value should be an integer
        const int _v_min, _v_max;
        int value_int = 0;
        // This is only used if value should be a string
        const std::function<bool(const std::string)> _validator;
        std::string value_string = "";

    public:
        ConfigValue(
            const std::string &key,
            const bool is_int, const bool optional,
            const int v_min = INT_MIN, const int v_max = INT_MAX, const int default_int = 0
        ) : _key(key), _is_int(is_int), _optional(optional), _v_min(v_min), _v_max(v_max), _validator(nullptr) {
            value_int = default_int;
            if (!_optional) value_string = std::to_string(default_int);
        }

        ConfigValue(
            const std::string &key,
            const bool is_int, const bool optional,
            const std::function<bool(const std::string)> validator = nullptr, const std::string _default = ""
        ) : _key(key), _is_int(is_int), _optional(optional), _v_min(INT_MIN), _v_max(INT_MAX), _validator(validator) {
            value_string = _default;
        }

		[[nodiscard]]
        bool get_int(int &out_value) const {
            if (!_is_int) return false;
            out_value = value_int;
            return true;
        }

        [[nodiscard]]
        std::string get_string() const {
            return value_string;
        }

        bool set(const int &value) {
            if (!_is_int) return false;
            if (value < _v_min || value > _v_max) return false;
            value_int = value;
            value_string = std::to_string(value);
            return true;
        }

        bool set(const std::string &value) {
			if(_is_int) try {
                return set(std::stoi(value));
			} catch (...) {
                return false;
            }
            if (value.empty()) {
                if (_optional) {
                    value_string = "";
                    return true;
                }
                return false;
            }
            if (_validator != nullptr && !_validator(value)) return false;
            value_string = value;
            return true;
        }

        [[nodiscard]] std::string key() const { return _key; }
        [[nodiscard]] bool optional() const { return _optional; }
    };

    class ConfigLoader {
    public:
        ConfigLoader() = default;
        ~ConfigLoader() = default;

        bool load(const std::string &path);
        bool save(const std::string &path);
    };

    class ThreadedConfigLoader : public ConfigLoader {
    public:
        bool load(const std::string &path);
        bool save(const std::string &path);

        bool is_finished() const { return _is_finished; }

    private:
        std::atomic_bool _is_finished = false;
    };
}