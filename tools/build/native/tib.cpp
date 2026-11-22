/* Validate actual x86 COFF storage and linked MAP ownership. */
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

static void require(bool ok, const char *message)
{
    if (!ok) throw std::runtime_error(message);
}

static std::string read(const wchar_t *path)
{
    std::ifstream file(std::filesystem::path(path), std::ios::binary);
    require(file.is_open(), "Cannot open input");
    std::string text((std::istreambuf_iterator<char>(file)), {});
    require(!file.bad(), "Cannot read input");
    return text;
}

struct coff {
    std::string bytes;
    void span(size_t at, size_t count) const
    {
        require(at <= bytes.size() && count <= bytes.size() - at, "Truncated COFF");
    }
    unsigned u16(size_t at) const
    {
        span(at, 2);
        return static_cast<unsigned char>(bytes[at]) |
            (static_cast<unsigned char>(bytes[at + 1]) << 8);
    }
    uint32_t u32(size_t at) const
    {
        return u16(at) | (static_cast<uint32_t>(u16(at + 2)) << 16);
    }
    std::string name(size_t at, size_t strings, size_t string_end) const
    {
        size_t end = at + 8;
        if (u32(at) == 0) {
            const auto offset = u32(at + 4);
            require(offset >= 4 && offset < string_end - strings, "Invalid COFF name offset");
            at = strings + offset;
            end = string_end;
        }
        size_t stop = at;
        while (stop < end && bytes[stop]) ++stop;
        require(end == at + 8 || stop < end, "Unterminated COFF name");
        return bytes.substr(at, stop - at);
    }
};

int wmain(int argc, wchar_t **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ntvdm-tib product.map dpmiint.obj\n";
        return 64;
    }
    try {
        const auto map = read(argv[1]);
        const coff object{read(argv[2])};
        require(object.u16(0) == 0x14c, "Requires x86 COFF");
        const size_t table = object.u32(8), count = object.u32(12);
        object.span(table, count * 18);
        const size_t strings = table + count * 18;
        const size_t string_size = object.u32(strings);
        require(string_size >= 4, "Invalid COFF string table");
        object.span(strings, string_size);
        struct symbol { unsigned section; uint32_t offset; };
        std::vector<symbol> symbols;
        unsigned section = 0, definitions = 0;
        uint32_t start = 0, end = 0;
        for (size_t i = 0; i < count;) {
            const size_t at = table + i * 18;
            const auto name = object.name(at, strings, strings + string_size);
            const auto part = object.u16(at + 12);
            const auto value = object.u32(at + 8);
            if (part > 0 && part < 0x8000 && object.bytes[at + 16] == 2)
                symbols.push_back({part, value});
            if (name == "_VdmTib") {
                require(part > 0 && part <= object.u16(2), "VdmTib must be a real section definition");
                ++definitions;
                section = part;
                start = value;
                const size_t header = 20 + object.u16(16) + (part - 1) * 40;
                object.span(header, 40);
                end = object.u32(header + 16);
            }
            const size_t advance = 1 + static_cast<unsigned char>(object.bytes[at + 17]);
            require(advance <= count - i, "Invalid COFF auxiliary count");
            i += advance;
        }
        require(definitions == 1, "Expected one VdmTib definition");
        for (const auto &s : symbols)
            if (s.section == section && s.offset > start && s.offset < end) end = s.offset;
        require(end > start && end - start > 0xfc0, "Full VDM_TIB must include printer fields");
        const uint64_t storage = end - start;
        const std::regex row(R"(^\s+[0-9a-fA-F]+:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{8})\s)");
        std::istringstream lines(map);
        std::string line;
        uint64_t address = 0;
        unsigned owners = 0;
        bool binding = false;
        std::vector<std::pair<std::string, uint64_t>> linked;
        while (std::getline(lines, line)) {
            std::smatch match;
            if (!std::regex_search(line, match, row)) continue;
            const auto name = match[1].str();
            const auto value = std::stoull(match[2].str(), nullptr, 16);
            if (name == "_VdmTib") {
                require(line.find("dpmiint.obj") != line.npos && line.find("<common>") == line.npos,
                        "Product must select dpmiint.obj, never COMMON storage");
                address = value;
                ++owners;
            } else linked.emplace_back(name, value);
            if (name == "_mvdm_monitor_bind_current_thread") binding = true;
        }
        require(owners == 1, "Expected one linked VdmTib owner");
        for (const auto &s : linked)
            require(s.second < address || s.second >= address + storage, "Linked symbol overlaps VdmTib storage");
        require(binding, "Startup binding must be linked");
        std::cout << "PASS: VdmTib owner=dpmiint.obj bytes=" << storage
                  << " address=0x" << std::hex << address << "; no overlapping symbol\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "VDM_TIB verification failed: " << error.what() << '\n';
        return 1;
    }
}
