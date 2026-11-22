/* Selected CCPU/C-VID metadata projection; no build orchestration. */
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace fs = std::filesystem;
using names = std::vector<std::string>;
using mapping = std::map<std::string, std::string>;
static constexpr auto rx_flags = std::regex_constants::ECMAScript;

static std::string read(const fs::path &path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Cannot read " + path.string());
    std::ostringstream result;
    result << input.rdbuf();
    if (input.bad()) throw std::runtime_error("Input read failed");
    return result.str();
}

static std::string trim(const std::string &s)
{
    const auto first = s.find_first_not_of(" \t\r\n");
    return first == s.npos ? "" : s.substr(first, s.find_last_not_of(" \t\r\n") - first + 1);
}

static std::string match(const std::string &text, const std::string &pattern, size_t group = 0)
{
    std::smatch found;
    if (!std::regex_search(text, found, std::regex(pattern, rx_flags)))
        throw std::runtime_error("Required original metadata missing: " + pattern);
    return found[group].str();
}

static names matches(const std::string &text, const std::string &pattern, size_t group = 0)
{
    const std::regex rx(pattern, rx_flags);
    names result;
    for (auto i = std::sregex_iterator(text.begin(), text.end(), rx); i != std::sregex_iterator(); ++i)
        result.push_back((*i)[group].str());
    return result;
}

static void changed(const fs::path &path, const std::string &text)
{
    if (fs::exists(path) && read(path) == text) return;
    const auto pending = fs::path(path.wstring() + L".tmp");
    try {
        std::ofstream output(pending, std::ios::binary | std::ios::trunc);
        output.write(text.data(), static_cast<std::streamsize>(text.size()));
        output.close();
        if (!output || !MoveFileExW(pending.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING))
            throw std::runtime_error("Cannot replace " + path.string());
    } catch (...) {
        DeleteFileW(pending.c_str());
        throw;
    }
}

static std::string emit(const std::string &macro, const names &slots, const mapping &special)
{
    std::string text = "#define " + macro + "(X) \\\n";
    for (size_t i = 0; i != slots.size(); ++i) {
        const auto &name = slots[i];
        const auto found = special.find(name);
        std::string provider;
        if (found != special.end()) provider = found->second;
        else {
            provider = name;
            provider[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(provider[0])));
            provider = "c_" + provider;
        }
        text += "    X(" + name + ", " + provider + ")";
        if (i + 1 != slots.size()) text += " \\\n";
    }
    return text + "\n";
}

int wmain(int argc, wchar_t **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ntvdm-cvidc output-header softpc-base-root\n";
        return 64;
    }
    try {
        const fs::path output(argv[1]), base(argv[2]);
        const auto metadata = read(base / "cvidc/c2cpusad.h");
        const std::regex group_rx(R"(char\s*\*\s*(\w+)\[\]\s*=\s*\{([\s\S]*?)\};)", rx_flags);
        std::map<std::string, names> groups;
        auto group = std::sregex_iterator(metadata.begin(), metadata.end(), group_rx);
        for (unsigned i = 0; i != 2; ++i, ++group) {
            if (group == std::sregex_iterator()) throw std::runtime_error("Missing original CPU vectors");
            groups[(*group)[1]] = matches((*group)[2], "\"([A-Za-z0-9_]+)\"", 1);
        }
        const mapping public_special = {
            {"Simulate","c_cpu_simulate"},{"Interrupt","c_cpu_interrupt"},
            {"ClearHwInt","0"},{"EndOfApplication","c_cpu_EOA_hook"},
            {"Terminate","c_cpu_terminate"},{"Initialise","c_cpu_init"},
            {"SetQuickEventCount","c_cpu_q_ev_set_count"},{"GetQuickEventCount","c_cpu_q_ev_get_count"},
            {"CalcQuickEventInstTime","c_cpu_calc_q_ev_inst_for_time"},{"InitIOS","0"},
            {"DefineInb","0"},{"DefineInw","0"},{"DefineInd","0"},
            {"DefineOutb","0"},{"DefineOutw","0"},{"DefineOutd","0"},
            {"GetJumpCalibrateVal","mvdm_cvidc_get_jump_calibration"},
            {"GetJumpInitialVal","mvdm_cvidc_get_jump_restart"},
            {"SetJumpInitialVal","mvdm_cvidc_set_jump_restart"},
            {"SetEOIEnable","setEOIEnableAddr"},{"SetAddProfileData","setAddProfileDataPtr"},
            {"SetMaxProfileData","setMaxProfileDataAddr"},{"GetAddProfileDataAddr","getAddProfileDataAddr"},
            {"PurgeLostIretHookLine","PurgeLostIretHookLine"},{"SetSTATUS","mvdm_cvidc_set_status"},
            {"SetAC","0"},{"GetAC","0"},{"GetET","0"},{"GetNE","0"},{"GetWP","0"}
        };
        const mapping private_special = {
            {"InitNanoCpu","0"},{"PrepareBlocksToCompile","0"},{"GrowRecPool","0"},
            {"BpiCompileBPI","0"},{"FmDeleteAllStructures","0"},{"GetSadInfoTable","0"},
            {"GetCpuState","0"},{"SetCpuState","0"},{"SetRegConstraint","0"},
            {"TrashIntelRegisters","0"},{"ConstraintsFromUnivEpcPtr","0"},{"ConstraintsFromUnivHandle","0"}
        };
        const auto &pub = groups.at("CpuVectorNames"), &priv = groups.at("CpuPrivateVectorNames");
        std::string binding = "/* Generated from original cvidc/c2cpusad.h. Do not edit. */\n";
        binding += "/* public=" + std::to_string(pub.size()) + "; private=" + std::to_string(priv.size()) + " */\n";
        binding += emit("MVDM_CVIDC_CPU_PUBLIC_SLOTS", pub, public_special);
        binding += emit("MVDM_CVIDC_CPU_PRIVATE_SLOTS", priv, private_special);
        const auto cpu = read(base / "cvidc/cpu4gen.h");
        auto declarations = matches(cpu, R"((?:^|\n)\s*IMPORT[^\r\n;]*\bc_[A-Za-z0-9_]+[^\r\n;]*;)");
        if (declarations.size() != 197) throw std::runtime_error("Original CPU declaration count changed");
        const auto sas_enum = match(read(base / "inc/sas.h"), R"(typedef\s+enum\s*\{\s*SAS_RAM\s*,[\s\S]*?\}\s*SAS_MEM_TYPE\s*;)");
        const auto sas = read(base.parent_path().parent_path().parent_path() / "compat/include/generated/x86/prod/sas4gen.h");
        const auto sas_vector = match(sas, R"(^[\s\S]*?extern\s+struct\s+SasVector\s+Sas\s*;)");
        if (std::regex_search(sas_vector, std::regex(R"((?:^|\n)\s*#\s*(if|ifdef|ifndef|elif)\b)", rx_flags)))
            throw std::runtime_error("Original SAS vector prefix changed");
        std::string providers = "/* Original selected CPU/SAS declarations; generated, do not edit. */\n";
        providers += sas_enum + "\n" + sas_vector + "\n";
        for (const auto &line : declarations) providers += trim(line) + "\n";
        const auto video = read(base / "cvidc/evidgen.h");
        const auto accessors = read(base / "cpu/src/evid/vglob.c");
        const auto setters = read(base / "cvidc/sinit011.c") + "\n" +
                             read(base / "cvidc/sinit012.c") + "\n" + read(base / "cvidc/sinit013.c");
        const std::regex accessor_rx(R"(setVideo(\w+) IFN1\(([^,]+), value\))", rx_flags);
        std::string cases = "/* Test offsets extracted from original generated setters, not vglob field layout. */\n";
        unsigned count = 0;
        for (auto i = std::sregex_iterator(accessors.begin(), accessors.end(), accessor_rx);
             i != std::sregex_iterator(); ++i) {
            const auto name = (*i)[1].str(), type = trim((*i)[2]);
            match(video, "([\\w]+(?:\\s*\\*)?)\\s*\\(\\*GetVideo" + name + "\\)", 1);
            const auto body = match(setters, "case\\s+S_\\d+_CiSetVideo" + name + "_\\w+_id\\s*:[\\s\\S]*?(?=case\\s|(?![\\s\\S]))");
            const auto offset = match(body, R"(&\(r20\)\)\s*=\s*\(IS32\)\((\d+)\))", 1);
            cases += "CHECK_FIELD(" + name + ", " + type + ", " + offset + ");\n";
            ++count;
        }
        if (count != 38) throw std::runtime_error("Original video accessor profile changed");
        const auto plane = match(read(base / "inc/egaports.h"), R"(#ifdef[ \t]+V7VGA[\s\S]*?(?:^|\n)(#define[ \t]+EGA_PLANE_SIZE[ \t]+[^\r\n]+))", 1);
        fs::create_directories(output.parent_path());
        changed(output, binding);
        changed(output.parent_path() / "cvidc_selected_providers.inc", providers);
        changed(output.parent_path() / "v7_selected_plane.h", plane + "\n");
        changed(output.parent_path() / "cvidc_accessor_cases.inc", cases);
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "CVID projection failed: " << error.what() << '\n';
        return 1;
    }
}
