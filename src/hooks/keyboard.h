#pragma once
#include <vector>
#include <QString>

#include "../common.h"

struct KeyboardRowData {
    const char* key;
    int code;
};

struct KeyboardMap {
    const char* parentKey;
    std::vector<std::vector<KeyboardRowData>> rows;
};

const std::vector<KeyboardMap> VIETNAMESE_KEYBOARD_MAP = {
    {"a", {
        {{"á", 0x1}, {"à", 0x2}, {"ả", 0x3}, {"ã", 0x4}, {"ạ", 0x5}},
        {{"â", 0x6}, {"ấ", 0x7}, {"ầ", 0x8}, {"ẩ", 0x9}, {"ẫ", 0x10}},
        {{"ă", 0x11}, {"ắ", 0x12}, {"ằ", 0x13}, {"ẳ", 0x14}, {"ẵ", 0x15}}
    }},
};
