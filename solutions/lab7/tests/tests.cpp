#include "../src/dragon.h"
#include "../src/elf.h"
#include "../src/knight_errant.h"
#include "../src/npc.h"
#include "gtest/gtest.h"

#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include <queue>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;
std::mutex print_mutex;
std::atomic<bool> stop_game{false};
std::shared_mutex npc_array_mutex;

class TextObserver : public IFightObserver {
private:
  TextObserver(){};

public:
  static std::shared_ptr<IFightObserver> get() {
    static TextObserver instance;
    return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
  }

  void on_fight(const std::shared_ptr<NPC> attacker, int attack_roll,
                const std::shared_ptr<NPC> defender, int defense_roll,
                bool win) override {
    std::lock_guard<std::mutex> lck(print_mutex);
    std::cout << "\nFight Details --------\n";
    std::cout << "Attacker:\n";
    attacker->print();
    std::cout << "Defender:\n";
    defender->print();
    std::cout << "Attack roll: " << attack_roll
              << ", Defense roll: " << defense_roll << '\n';
    std::cout << (win ? "Defender died!" : "Defender survived.") << "\n\n";
  }
};

std::shared_ptr<NPC> factory(std::istream &is) {
  std::shared_ptr<NPC> result;
  int type{0};
  if (is >> type) {
    switch (type) {
    case DragonType:
      result = std::make_shared<Dragon>(is);
      break;
    case ElfType:
      result = std::make_shared<Elf>(is);
      break;
    case KnightErrantType:
      result = std::make_shared<KnightErrant>(is);
      break;
    }
  } else
    std::cerr << "unexpected NPC type:" << type << '\n';

  if (result)
    result->subscribe(TextObserver::get());

  return result;
}

std::shared_ptr<NPC> factory(NpcType type, int x, int y) {
  std::shared_ptr<NPC> result;
  switch (type) {
  case DragonType:
    result = std::make_shared<Dragon>(x, y);
    break;
  case ElfType:
    result = std::make_shared<Elf>(x, y);
    break;
  case KnightErrantType:
    result = std::make_shared<KnightErrant>(x, y);
    break;
  default:
    break;
  }
  if (result)
    result->subscribe(TextObserver::get());

  return result;
}

void save(const set_t &array, const std::string &filename) {
  std::ofstream fs(filename);
  fs << array.size() << '\n';
  for (auto &n : array)
    n->save(fs);
  fs.flush();
  fs.close();
}

set_t load(const std::string &filename) {
  set_t result;
  std::ifstream is(filename);
  if (is.good() && is.is_open()) {
    int count;
    is >> count;
    for (int i = 0; i < count; ++i)
      result.insert(factory(is));
    is.close();
  } else
    std::cerr << "Error: " << std::strerror(errno) << '\n';
  return result;
}

std::ostream &operator<<(std::ostream &os, const set_t &array) {
  for (auto &n : array)
    n->print();
  return os;
}

set_t fight(const set_t &array, size_t distance) {
  set_t dead_list;

  for (const auto &attacker : array)
    for (const auto &defender : array)
      if ((attacker != defender) && attacker->is_close(defender, distance) &&
          defender->accept(attacker))
        dead_list.insert(defender);

  return dead_list;
}

struct print : std::stringstream {
  ~print() {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lck(print_mutex);
    std::cout << this->str();
    std::cout.flush();
  }
};

struct FightEvent {
  std::shared_ptr<NPC> attacker;
  std::shared_ptr<NPC> defender;
};

class FightManager {
private:
  std::queue<FightEvent> events;
  FightManager() {}
  std::mutex mtx;

public:
  static FightManager &get() {
    static FightManager instance;
    return instance;
  }

  void add_event(FightEvent &&event) {
    std::lock_guard<std::mutex> lck(mtx);
    events.push(event);
  }

  void operator()() {
    while (!stop_game) {
      std::optional<FightEvent> event;

      {
        std::lock_guard<std::mutex> lck(mtx);
        if (!events.empty()) {
          event = events.front();
          events.pop();
        }
      }

      if (event) {
        auto attacker = event->attacker;
        auto defender = event->defender;

        if (attacker->is_alive() && defender->is_alive() &&
            defender->accept(attacker)) {
          int attack_roll = std::rand() % 6 + 1;
          int defense_roll = std::rand() % 6 + 1;

          bool win = attack_roll > defense_roll;

          if (win) {
            defender->must_die();
          }

          TextObserver::get()->on_fight(attacker, attack_roll, defender,
                                        defense_roll, win);
        }
      }

      std::this_thread::sleep_for(100ms);
    }
  }
};

TEST(Lab7Test, Factory) {
  std::stringstream ss("1 10 20");
  auto dragon = factory(ss);
  ASSERT_NE(dragon, nullptr);
  ASSERT_EQ(dragon->get_type(), DragonType);

  std::stringstream ss2("2 30 40");
  auto elf = factory(ss2);
  ASSERT_NE(elf, nullptr);
  ASSERT_EQ(elf->get_type(), ElfType);

  auto knight = factory(KnightErrantType, 50, 60);
  ASSERT_NE(knight, nullptr);
  ASSERT_EQ(knight->get_type(), KnightErrantType);
}

TEST(Lab7Test, SaveLoad) {
  set_t initial_npcs;
  initial_npcs.insert(
      std::allocate_shared<Dragon>(std::allocator<Dragon>(), 10, 20));
  initial_npcs.insert(std::allocate_shared<Elf>(std::allocator<Elf>(), 30, 40));
  initial_npcs.insert(std::make_shared<KnightErrant>(50, 60));

  const std::string filename = "test_npcs.txt";
  save(initial_npcs, filename);

  set_t loaded_npcs = load(filename);

  ASSERT_EQ(initial_npcs.size(), loaded_npcs.size());
}

TEST(Lab7Test, FightManager) {
  FightManager &fm = FightManager::get();
  std::shared_ptr<NPC> attacker =
      std::allocate_shared<Dragon>(std::allocator<Dragon>(), 0, 0);
  std::shared_ptr<NPC> defender = std::make_shared<KnightErrant>(1, 1);

  fm.add_event({attacker, defender});
  stop_game = false;
  std::thread fight_thread(std::ref(fm));
  std::this_thread::sleep_for(200ms);
  stop_game = true;
  fight_thread.join();
}

TEST(NpcTest, IsClose) {
  std::shared_ptr<NPC> npc1 =
      std::allocate_shared<Dragon>(std::allocator<Dragon>(), 0, 0);
  std::shared_ptr<NPC> npc2 =
      std::allocate_shared<Elf>(std::allocator<Elf>(), 3, 4);
  ASSERT_TRUE(npc1->is_close(npc2, 5));
  ASSERT_FALSE(npc1->is_close(npc2, 4));
}

TEST(NpcTest, AliveStatus) {
  Elf elf(0, 0);
  ASSERT_TRUE(elf.is_alive());
  elf.must_die();
  ASSERT_FALSE(elf.is_alive());
}

TEST(DragonTest, Accept) {
  std::shared_ptr<NPC> dragon =
      std::allocate_shared<Dragon>(std::allocator<Dragon>(), 0, 0);
  std::shared_ptr<NPC> elf =
      std::allocate_shared<Elf>(std::allocator<Elf>(), 0, 0);
  std::shared_ptr<NPC> knight = std::make_shared<KnightErrant>(0, 0);

  ASSERT_TRUE(dragon->accept(dragon));
  ASSERT_TRUE(dragon->accept(elf));
  ASSERT_TRUE(dragon->accept(knight));
}

TEST(ElfTest, Accept) {
  std::shared_ptr<NPC> elf =
      std::allocate_shared<Elf>(std::allocator<Elf>(), 0, 0);
  std::shared_ptr<NPC> knight = std::make_shared<KnightErrant>(0, 0);
  std::shared_ptr<NPC> dragon =
      std::allocate_shared<Dragon>(std::allocator<Dragon>(), 0, 0);

  ASSERT_TRUE(elf->accept(knight));
  ASSERT_FALSE(elf->accept(elf));
  ASSERT_FALSE(elf->accept(dragon));
}

TEST(KnightErrantTest, Accept) {
  std::shared_ptr<NPC> knight = std::make_shared<KnightErrant>(0, 0);
  std::shared_ptr<NPC> dragon =
      std::allocate_shared<Dragon>(std::allocator<Dragon>(), 0, 0);
  std::shared_ptr<NPC> elf =
      std::allocate_shared<Elf>(std::allocator<Elf>(), 0, 0);

  ASSERT_TRUE(knight->accept(dragon));
  ASSERT_FALSE(knight->accept(knight));
  ASSERT_FALSE(knight->accept(elf));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
