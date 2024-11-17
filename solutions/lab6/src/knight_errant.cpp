#include "knight_errant.h"
#include "dragon.h"
#include "elf.h"

KnightErrant::KnightErrant(int x, int y) : NPC(KnightErrantType, x, y) {}
KnightErrant::KnightErrant(std::istream &is) : NPC(KnightErrantType, is) {}

void KnightErrant::print() { std::cout << *this; }

bool KnightErrant::accept(Visitor &visitor) { return visitor.visit(*this); }

bool KnightErrant::fight(std::shared_ptr<KnightErrant> other) {
  fight_notify(other, false);
  return false;
}

bool KnightErrant::fight(std::shared_ptr<Elf> other) {
  fight_notify(other, false);
  return false;
}

bool KnightErrant::fight(std::shared_ptr<Dragon> other) {
  fight_notify(other, true);
  return true;
}

void KnightErrant::save(std::ostream &os) {
  os << KnightErrantType << std::endl;
  NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, KnightErrant &knight_errant) {
  os << "Knight Errant: " << *static_cast<NPC *>(&knight_errant) << std::endl;
  return os;
}
