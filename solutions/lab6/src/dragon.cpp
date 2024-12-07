#include "dragon.h"
#include "elf.h"
#include "knight_errant.h"

Dragon::Dragon(int x, int y) : NPC(DragonType, x, y) {}
Dragon::Dragon(std::istream &is) : NPC(DragonType, is) {}

void Dragon::print() { std::cout << *this; }

bool Dragon::accept(Visitor &visitor) { return visitor.visit(*this); }

bool Dragon::fight(std::shared_ptr<KnightErrant> other) {
  fight_notify(other, true);
  return true;
}

bool Dragon::fight(std::shared_ptr<Elf> other) {
  fight_notify(other, true);
  return true;
}

bool Dragon::fight(std::shared_ptr<Dragon> other) {
  fight_notify(other, true);
  return true;
}

void Dragon::save(std::ostream &os) {
  os << DragonType << std::endl;
  NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, Dragon &dragon) {
  os << "Dragon: " << *static_cast<NPC *>(&dragon) << std::endl;
  return os;
}
