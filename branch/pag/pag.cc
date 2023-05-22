#include <algorithm>
#include <array>
#include <bitset>
#include <map>

#include "msl/fwcounter.h"
#include "ooo_cpu.h"

namespace
{
constexpr std::size_t COUNTER_BITS = 2;
constexpr std::size_t LOCAL_HISTORY_LENGTH = 14;
constexpr std::size_t GLOBAL_HISTORY_TABLE_SIZE = 16384;

std::map<O3_CPU*, std::array<champsim::msl::fwcounter<COUNTER_BITS>, GLOBAL_HISTORY_TABLE_SIZE>> global_history;
std::map<O3_CPU*, std::map<uint64_t, std::bitset<LOCAL_HISTORY_LENGTH>>> local_history_table;

} // namespace

void O3_CPU::initialize_branch_predictor() { std::cout << "CPU " << cpu << " PAg branch predictor" << std::endl; }

uint8_t O3_CPU::predict_branch(uint64_t ip)
{
  auto hash = ::local_history_table[this][ip];
  auto value = ::global_history[this][hash.to_ulong()];
  return value.value() >= (value.maximum / 2);
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  auto hash = ::local_history_table[this][ip];
  ::global_history[this][hash.to_ulong()] += taken ? 1 : -1;

  // update local history vector
  ::local_history_table[this][ip] <<= 1;
  ::local_history_table[this][ip][0] = taken;
}
