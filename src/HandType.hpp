// HandType.hpp
// Enumeracia pre rozlisenie lavej a pravej rukavice.
// Lava rukavica prepina rezimy riadenia robota, prava rukavica vykonava prikazy.

#pragma once
#ifndef HANDTYPE_HPP
#define HANDTYPE_HPP

enum class HandType {
	Left,	// lava rukavica - prepinanie rezimov
	Right	// prava rukavica - vykonavanie prikazov

};

#endif // !HANDTYPE_HPP

