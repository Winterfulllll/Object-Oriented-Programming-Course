#include "src/dragon.h"
#include "src/elf.h"
#include "src/knight_errant.h"
#include "src/npc.h"

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

int main() {
  const int MAX_X{100}, MAX_Y{100}, DISTANCE{50}, GAME_DURATION{30};
  const int GRID{20}, STEP_X{MAX_X / GRID}, STEP_Y{MAX_Y / GRID};
  set_t array;

  std::cout << "Generating initial NPCs...\n";
  for (size_t i = 0; i < 50; ++i)
    array.insert(factory(NpcType(std::rand() % 3 + 1), std::rand() % MAX_X,
                         std::rand() % MAX_Y));

  std::cout << "Initial list of NPCs:\n" << array;

  std::thread fight_thread(std::ref(FightManager::get()));

  std::thread move_thread([&]() {
    while (!stop_game) {
      {
        std::shared_lock lock(npc_array_mutex);
        for (const auto &npc : array) {
          if (npc->is_alive()) {
            int move_distance = 0;
            switch (npc->get_type()) {
            case KnightErrantType:
              move_distance = 30;
              break;
            case ElfType:
              move_distance = 10;
              break;
            case DragonType:
              move_distance = 50;
              break;
            default:
              break;
            }

            int dx = std::rand() % (2 * move_distance + 1) - move_distance;
            int dy = std::rand() % (2 * move_distance + 1) - move_distance;

            npc->move(dx, dy, MAX_X, MAX_Y);
          }
        }

        for (const auto &npc : array) {
          for (const auto &other : array) {
            if (npc != other && npc->is_alive() && other->is_alive()) {
              int kill_distance = 0;
              switch (npc->get_type()) {
              case KnightErrantType:
                kill_distance = 10;
                break;
              case ElfType:
                kill_distance = 50;
                break;
              case DragonType:
                kill_distance = 30;
                break;
              default:
                break;
              }

              if (npc->is_close(other, kill_distance)) {
                FightManager::get().add_event({npc, other});
              }
            }
          }
        }
      }
      std::this_thread::sleep_for(10ms);
    }
  });

  auto start_time = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - start_time <
         std::chrono::seconds(GAME_DURATION)) {
    std::array<char, GRID * GRID> fields{};
    fields.fill(0);

    for (const auto &npc : array) {
      const auto [x, y] = npc->position();
      int i = x / STEP_X;
      int j = y / STEP_Y;

      if (npc->is_alive()) {
        switch (npc->get_type()) {
        case DragonType:
          fields[i + GRID * j] = 'D';
          break;
        case ElfType:
          fields[i + GRID * j] = 'E';
          break;
        case KnightErrantType:
          fields[i + GRID * j] = 'K';
          break;
        default:
          break;
        }
      } else {
        fields[i + GRID * j] = '.';
      }
    }

    {
      std::lock_guard<std::mutex> lck(print_mutex);
      for (int j = 0; j < GRID; ++j) {
        for (int i = 0; i < GRID; ++i) {
          char c = fields[i + j * GRID];
          std::cout << "[" << (c ? c : ' ') << "]";
        }
        std::cout << '\n';
      }
      std::cout << "\n";
    }
    std::this_thread::sleep_for(1s);
  }

  stop_game = true;
  fight_thread.join();
  move_thread.join();

  std::cout << "Survivors:\n";
  for (const auto &npc : array) {
    if (npc->is_alive()) {
      npc->print();
    }
  }
  std::cout << "\n\n";

  return 0;
}
