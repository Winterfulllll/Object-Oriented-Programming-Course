#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <memory>
#include <random>
#include <set>
#include <string>

struct NPC;
struct KnightErrant;
struct Elf;
struct Dragon;
using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType { Unknown = 0, KnightErrantType = 1, ElfType = 2, DragonType = 3 };

struct IFightObserver {
  virtual void on_fight(const std::shared_ptr<NPC> attacker,
                        const std::shared_ptr<NPC> defender, bool win) = 0;
};

class Visitor {
public:
  virtual bool visit(KnightErrant &knight) = 0;
  virtual bool visit(Elf &elf) = 0;
  virtual bool visit(Dragon &dragon) = 0;
  virtual ~Visitor() = default;
};

struct NPC : public std::enable_shared_from_this<NPC> {
  NpcType type;
  int x{0};
  int y{0};
  std::vector<std::shared_ptr<IFightObserver>> observers;

  NPC(NpcType t, int _x, int _y);
  NPC(NpcType t, std::istream &is);

  void subscribe(std::shared_ptr<IFightObserver> observer);
  void fight_notify(const std::shared_ptr<NPC> defender, bool win);
  virtual bool is_close(const std::shared_ptr<NPC> &other,
                        size_t distance) const;

  virtual bool accept(Visitor &visitor) = 0;

  virtual bool fight(std::shared_ptr<KnightErrant> other) = 0;
  virtual bool fight(std::shared_ptr<Elf> other) = 0;
  virtual bool fight(std::shared_ptr<Dragon> other) = 0;
  virtual void print() = 0;

  virtual void save(std::ostream &os);

  friend std::ostream &operator<<(std::ostream &os, NPC &npc);
};
