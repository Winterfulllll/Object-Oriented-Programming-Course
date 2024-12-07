#include "../src/dragon.h"
#include "../src/elf.h"
#include "../src/knight_errant.h"
#include "../src/npc.h"

#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

// Mock Observer for testing
class MockFightObserver : public IFightObserver {
public:
  int fight_count = 0;
  std::shared_ptr<NPC> last_attacker;
  std::shared_ptr<NPC> last_defender;
  bool last_win;

  void on_fight(const std::shared_ptr<NPC> attacker,
                const std::shared_ptr<NPC> defender, bool win) override {
    fight_count++;
    last_attacker = attacker;
    last_defender = defender;
    last_win = win;
  }
};

class TextObserver : public IFightObserver {
private:
  TextObserver(){};

public:
  static std::shared_ptr<IFightObserver> get() {
    static TextObserver instance;
    return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
  }

  void on_fight(const std::shared_ptr<NPC> attacker,
                const std::shared_ptr<NPC> defender, bool win) override {
    if (win) {
      std::cout << std::endl << "Murder --------" << std::endl;
      attacker->print();
      defender->print();
    }
  }
};

std::shared_ptr<NPC> factory(std::istream &is) {
  std::shared_ptr<NPC> result;
  int type{0};
  if (is >> type) {
    switch (type) {
    case KnightErrantType:
      result = std::make_shared<KnightErrant>(is);
      break;
    case ElfType:
      result = std::make_shared<Elf>(is);
      break;
    case DragonType:
      result = std::make_shared<Dragon>(is);
      break;
    }
  } else
    std::cerr << "unexpected NPC type:" << type << std::endl;

  if (result)
    result->subscribe(TextObserver::get());

  return result;
}

std::shared_ptr<NPC> factory(NpcType type, int x, int y) {
  std::shared_ptr<NPC> result;
  switch (type) {
  case KnightErrantType:
    result = std::make_shared<KnightErrant>(x, y);
    break;
  case ElfType:
    result = std::make_shared<Elf>(x, y);
    break;
  case DragonType:
    result = std::make_shared<Dragon>(x, y);
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
  fs << array.size() << std::endl;
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
    std::cerr << "Error: " << std::strerror(errno) << std::endl;
  return result;
}

std::ostream &operator<<(std::ostream &os, const set_t &array) {
  for (auto &n : array)
    n->print();
  return os;
}

class AttackerVisitor : public Visitor {
public:
  AttackerVisitor(const std::shared_ptr<NPC> &attacker) : attacker_(attacker) {}

  bool visit(KnightErrant &knight) override {
    return attacker_->fight(std::make_shared<KnightErrant>(knight));
  }
  bool visit(Elf &elf) override {
    return attacker_->fight(std::make_shared<Elf>(elf));
  }
  bool visit(Dragon &dragon) override {
    return attacker_->fight(std::make_shared<Dragon>(dragon));
  }

private:
  std::shared_ptr<NPC> attacker_;
};

set_t fight(const set_t &array, size_t distance) {
  set_t dead_list;

  for (const auto &attacker : array) {
    for (const auto &defender : array) {
      if (attacker != defender && attacker->is_close(defender, distance)) {
        AttackerVisitor visitor(attacker);
        if (defender->accept(visitor)) {
          dead_list.insert(defender);
        }
      }
    }
  }
  return dead_list;
}

TEST(FactoryTest, CreateKnight) {
  auto knight = factory(KnightErrantType, 10, 20);
  ASSERT_NE(knight, nullptr);
  ASSERT_EQ(knight->type, KnightErrantType);
  ASSERT_EQ(knight->x, 10);
  ASSERT_EQ(knight->y, 20);
}

TEST(FactoryTest, CreateElf) {
  auto elf = factory(ElfType, 30, 40);
  ASSERT_NE(elf, nullptr);
  ASSERT_EQ(elf->type, ElfType);
  ASSERT_EQ(elf->x, 30);
  ASSERT_EQ(elf->y, 40);
}

TEST(FactoryTest, CreateDragon) {
  auto dragon = factory(DragonType, 50, 60);
  ASSERT_NE(dragon, nullptr);
  ASSERT_EQ(dragon->type, DragonType);
  ASSERT_EQ(dragon->x, 50);
  ASSERT_EQ(dragon->y, 60);
}

TEST(FactoryTest, LoadKnight) {
  std::stringstream ss("1\n10\n20\n");
  auto knight = factory(ss);
  ASSERT_NE(knight, nullptr);
  ASSERT_EQ(knight->type, KnightErrantType);
  ASSERT_EQ(knight->x, 10);
  ASSERT_EQ(knight->y, 20);
}

TEST(FightTest, DragonKillsAll) {
  auto dragon = std::make_shared<Dragon>(0, 0);
  auto knight = std::make_shared<KnightErrant>(0, 0);
  auto elf = std::make_shared<Elf>(0, 0);

  ASSERT_TRUE(dragon->fight(knight));
  ASSERT_TRUE(dragon->fight(elf));
  ASSERT_TRUE(dragon->fight(dragon));
}

TEST(FightTest, KnightKillsDragon) {
  auto knight = std::make_shared<KnightErrant>(0, 0);
  auto dragon = std::make_shared<Dragon>(0, 0);

  ASSERT_TRUE(knight->fight(dragon));
}

TEST(FightTest, ElfKillsKnight) {
  auto elf = std::make_shared<Elf>(0, 0);
  auto knight = std::make_shared<KnightErrant>(0, 0);

  ASSERT_TRUE(elf->fight(knight));
}

TEST(FightTest, DistanceCheck) {
  auto knight1 = std::make_shared<KnightErrant>(0, 0);
  auto knight2 = std::make_shared<KnightErrant>(4, 3);

  ASSERT_TRUE(knight1->is_close(knight2, 5));
  ASSERT_FALSE(knight1->is_close(knight2, 4));
}

TEST(ObserverTest, FightNotification) {
  auto mockObserver = std::make_shared<MockFightObserver>();
  auto knight = std::make_shared<KnightErrant>(0, 0);
  auto dragon = std::make_shared<Dragon>(0, 0);

  knight->subscribe(mockObserver);
  knight->fight(dragon);

  ASSERT_EQ(mockObserver->fight_count, 1);
  ASSERT_EQ(mockObserver->last_attacker, knight);
  ASSERT_EQ(mockObserver->last_defender, dragon);
  ASSERT_TRUE(mockObserver->last_win);
}

TEST(SaveLoadTest, SaveAndLoad) {
  set_t array;
  array.insert(std::make_shared<Dragon>(10, 20));
  array.insert(std::make_shared<KnightErrant>(30, 40));

  save(array, "test_save.txt");

  set_t loaded_array = load("test_save.txt");

  ASSERT_EQ(array.size(), loaded_array.size());

  std::remove("test_save.txt");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
