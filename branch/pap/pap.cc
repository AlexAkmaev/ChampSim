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
constexpr std::size_t LOCAL_HISTORY_TABLE_SIZE = 16384;

using per_adrr_pht = std::array<champsim::msl::fwcounter<COUNTER_BITS>, LOCAL_HISTORY_TABLE_SIZE>;
std::map<O3_CPU*, std::map<uint64_t, per_adrr_pht>> pattern_history_table;
std::map<O3_CPU*, std::map<uint64_t, std::bitset<LOCAL_HISTORY_LENGTH>>> local_history_table;

} // namespace

void O3_CPU::initialize_branch_predictor() { std::cout << "CPU " << cpu << " PAp branch predictor" << std::endl; }

uint8_t O3_CPU::predict_branch(uint64_t ip)
{
  auto bhr_hash = ::local_history_table[this][ip];
  auto pht = ::pattern_history_table[this][ip];
  auto value = pht[bhr_hash.to_ulong()];
  return value.value() >= (value.maximum / 2);
}

void O3_CPU::last_branch_result(uint64_t ip, uint64_t branch_target, uint8_t taken, uint8_t branch_type)
{
  auto bhr_hash = ::local_history_table[this][ip];
  ::pattern_history_table[this][ip][bhr_hash.to_ulong()] += taken ? 1 : -1;

  // update local history vector
  ::local_history_table[this][ip] <<= 1;
  ::local_history_table[this][ip][0] = taken;
}
